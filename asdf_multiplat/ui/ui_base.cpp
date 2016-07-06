#pragma clang diagnostic ignored "-Wunused-parameter"

#include "stdafx.h"

#include "ui_base.h"
#include "ui_button.h"
#include "utilities.h"
#include "spritebatch.h"

#include "asdf_multiplat.h"
#include "content_manager.h"

using namespace glm;
using namespace std;
using namespace asdf::util;

namespace asdf
{
    aabb_t get_bounds(std::vector<aabb_t> const& aabbs)
    {
        if(aabbs.size() == 0)
        {
            return aabb_t{vec3(0.0f),vec3(0.0f)};
        }

        glm::vec3 _min = aabbs[0].lower_bound;
        glm::vec3 _max = aabbs[0].upper_bound;

        for(auto const& aabb : aabbs)
        {
            _min = glm::min(_min, aabb.lower_bound);
            _max = glm::max(_max, aabb.upper_bound);
        }

        return aabb_t{_min, _max};
    }

    /************************************************************************/
    /* UI Base
    /************************************************************************/
    ui_base_t::ui_base_t(vec3 const& position, mat3 const& matrix/*mat3()*/)
        : position(position)
        , offset(position)
        , matrix(matrix)
        , color(1.0f)
        , alignment(ui_align_none)
        , parent(nullptr)
    {

    }

    ui_base_t::~ui_base_t()
    {}

    ui_base_t* ui_base_t::add_child(shared_ptr<ui_base_t> child, size_t index/*nullindex*/)
    {
        child->parent = this;

        if (index == nullindex)
        {
            children.push_back(std::move(child));
            return children.back().get();
        }
        else
        {
            children.insert(children.begin() + index, std::move(child));
            return children[index].get();
        }
    }

    ui_base_t* ui_base_t::top_parent()
    {
        ui_base_t* topmost = this;
        while(topmost->parent)
        {
            topmost = topmost->parent;
        }

        return topmost;
    }

    void ui_base_t::align()
    {
        vec3 final_pos = offset;
        vec3 final_size = upper_bound - lower_bound;
        vec3 final_halfsize = final_size / 2.0f;

        if(parent && alignment > ui_align_center)
        {
            vec3 parent_size     = parent->upper_bound - parent->lower_bound;
            vec3 parent_halfsize = parent_size / 2.0f;

            // if ((alignment & ui_align_center) > 0)
            //     final_pos += parent->lower_bound + (parent->upper_bound - parent->lower_bound) / 2.0f;

            //vertical
            if ((alignment & ui_align_top) > 0) {
                //vertical stretch
                if ((alignment & ui_align_bottom) > 0) {
                    final_size.y = parent_size.y;
                }
                //top align
                else
                {
                    final_pos.y += parent_halfsize.y;
                    final_pos.y += final_halfsize.y * (alignment_edge == ui_align_edge_outside);
                    final_pos.y -= final_halfsize.y * (alignment_edge == ui_align_edge_inside);
                }
            }
            //bottom align
            else if ((alignment & ui_align_bottom) > 0)
            {
                final_pos.y -= parent_halfsize.y;
                final_pos.y -= final_halfsize.y * (alignment_edge == ui_align_edge_outside);
                final_pos.y += final_halfsize.y * (alignment_edge == ui_align_edge_inside);
            }


            //horizontal
            if ((alignment & ui_align_left) > 0) {
                //horizontal stretch
                if ((alignment & ui_align_right) > 0) 
                {
                    final_size.x = parent_size.x;
                }
                //left align
                else
                {
                    final_pos.x -= parent_halfsize.x;
                    final_pos.x -= final_halfsize.x * (alignment_edge == ui_align_edge_outside);
                    final_pos.x += final_halfsize.x * (alignment_edge == ui_align_edge_inside);
                }
            }
            //right align
            else if ((alignment & ui_align_right) > 0)
            {
                final_pos.x += parent_halfsize.x;
                final_pos.x += final_halfsize.x * (alignment_edge == ui_align_edge_outside);
                final_pos.x -= final_halfsize.x * (alignment_edge == ui_align_edge_inside);
            }
        }
        
        position = final_pos;

        auto pos = get_world_position();

        size3 = final_size;
        halfsize3 = final_size / 2.0f;

        final_size += expansion_delta / 2.0f;
        auto halfsize = final_size / 2.0f;

        upper_bound = pos + halfsize;
        lower_bound = pos - halfsize;

        for (auto const& child : children)
        {
            child->align();
        }
    }

    void ui_base_t::set_size(vec3 size)
    {
        auto halfsize = size / 2.0f;

        auto pos = get_world_position();
        upper_bound = pos + halfsize;
        lower_bound = pos - halfsize;
    }

    void ui_base_t::resize_to_fit()
    {
        auto bounds = calc_aabb();
        set_size(bounds.upper_bound - bounds.lower_bound);
    }

    void ui_base_t::update(float dt)
    {
        for (auto const& child : children)
            child->update(dt);
    }

    void ui_base_t::render(vec3 const& _position, mat3 const& _matrix, color_t const& _color)
    {
        if (children.size() == 0 || !visible)
            return;

        DEFAULT_TRANSFORM_COPYPASTA

        if (color_.a == 0.0f)
            return;

        for (auto const& child : children)
            if(child->visible) //not sure if I should check here or check in each child render call and return. or both
                child->render(position_, matrix_, color_);
    }

    void ui_base_t::render_debug()
    {
        vec3 hsize = (upper_bound - lower_bound) / 2.0f;
        vec3 pos = (lower_bound + hsize);

        //lazy debug draw
        glBegin(GL_POLYGON);
        glVertex2f(pos.x - hsize.x, pos.y + hsize.y);
        glVertex2f(pos.x + hsize.x, pos.y + hsize.y);
        glVertex2f(pos.x + hsize.x, pos.y - hsize.y);
        glVertex2f(pos.x - hsize.x, pos.y - hsize.y);
        glEnd();

        hsize -= 2;
        glBegin(GL_POLYGON);
        glVertex2f(pos.x - hsize.x, pos.y + hsize.y);
        glVertex2f(pos.x + hsize.x, pos.y + hsize.y);
        glVertex2f(pos.x + hsize.x, pos.y - hsize.y);
        glVertex2f(pos.x - hsize.x, pos.y - hsize.y);
        glEnd();

        for (auto const& child : children)
            child->render_debug();
    }

    bool ui_base_t::on_event(SDL_Event* event, glm::vec3 const& _position, glm::mat3 const& _matrix)
    {
        if ((event->type & SDL_MOUSEMOTION) > 0)
        {
            //todo: project ray from XY

            for (auto const& child : children)
            {
                if(child->enabled && child->on_event(event, _position + position, _matrix * matrix))
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ui_base_t::contains_point(vec3 point)
    {
        auto ub = position + halfsize3;
        auto lb = position - halfsize3;

        return check_bounds<float>(point.x, lb.x, ub.x)
            && check_bounds<float>(point.y, lb.y, ub.y)
            && check_bounds<float>(point.z, lb.z, ub.z)
            ;
    }

    vec3 ui_base_t::get_world_position()
    {
        vec3 position_ = position;

        ui_base_t* temp = parent;
        while (temp != nullptr) {
            position_ += temp->position;
            temp = temp->parent;
        }

        return position_;
    }

    mat4 ui_base_t::get_world_transform()
    {
        ASSERT(false, "Todo: this function");

        return mat4();
    }

    aabb_t ui_base_t::calc_aabb()
    {
        if(children.size() > 1)
        {
            std::vector<aabb_t> children_aabbs;

            for(auto const& child : children)
            {
                ASSERT(child, "null child");
                children_aabbs.push_back(child->calc_aabb());
            }

            return get_bounds(children_aabbs);
        }
        else
        {
            return aabb_t{halfsize3,-halfsize3};
        }
    }


    /************************************************************************/
    /* UI View
    /************************************************************************/
    ui_view_t::ui_view_t(vec2 const& _position, ui_alignment_e _alignment)
    : ui_view_t(_position, vec2(0.0f), _alignment)
    {}

    ui_view_t::ui_view_t(vec2 const& _position, vec2 const& size, ui_alignment_e alignment)
        : ui_base_t(vec3(_position, 0.0f))
        , size(size)
        , halfsize(size / 2.0f)
    {
        this->alignment = alignment;

        upper_bound = glm::vec3(_position + halfsize, 0);
        lower_bound = glm::vec3(_position - halfsize, 0);
    }

    void ui_view_t::align()
    {
        //auto pos = get_world_position();
        upper_bound = vec3( halfsize, 0.0f);
        lower_bound = vec3(-halfsize, 0.0f);
        ui_base_t::align();

        size = (upper_bound - lower_bound).xy;
        halfsize = size / 2.0f;
    }

    void ui_view_t::set_size(vec2 _size)
    {
        size = _size;
        halfsize = size / 2.0f;
        ui_base_t::set_size(vec3(size, 0.0f));
    }

    void ui_view_t::set_size(vec3 size)
    {
        halfsize = (size / 2.0f).xy;
        ui_base_t::set_size(size);
    }

    void ui_view_t::update(float dt)
    {
        ui_base_t::update(dt);
    }

    void ui_view_t::render(vec3 const& _position, mat3 const& _matrix, color_t const& _color)
    {
        ui_base_t::render(_position, _matrix, _color);
    }


    /************************************************************************/
    /* UI Image
    /************************************************************************/
    ui_image_t::ui_image_t(std::shared_ptr<texture_t> texture, glm::vec2 position, glm::vec2 size/*vec2(nullindex)*/, ui_alignment_e alignment/*align_none*/)
        : ui_view_t(position, size, alignment)
        , texture(texture)
    {
        if (size.x == nullindex && size.y == nullindex) {
            size.x = texture->get_width();
            size.y = texture->get_height();
        }
    }

    ui_image_t::ui_image_t(color_t color, glm::vec2 position, glm::vec2 size/*vec2(nullindex)*/, ui_alignment_e alignment/*align_none*/)
        : ui_view_t(position, size, alignment)
        , texture(Content.textures["pixel"])
    {
        this->color = color;
    }

    ui_image_t::ui_image_t(color_t color, ui_alignment_e alignment/*fill*/)
        : ui_view_t(vec2(0.0f), vec2(0.0f), alignment)
        , texture(Content.textures["pixel"])
    {
        this->color = color;
    }

    void ui_image_t::update(float dt)
    {
        ui_view_t::update(dt);
    }

    void ui_image_t::render(vec3 const& _position, mat3 const& _matrix, color_t const& _color)
    {
        DEFAULT_TRANSFORM_COPYPASTA

        if (color_.a == 0.0f)
            return;

        if (size.x == 5)
            int asdf = 9001;

        app.spritebatch->draw(texture, rectf_t(position_.xy, size), color_);

        ui_view_t::render(_position, _matrix, _color);
    }

    /************************************************************************/
    /* UI Slider
    /************************************************************************/
    ui_slider_t::ui_slider_t(std::shared_ptr<ui_button_t> button/*nullptr*/, std::shared_ptr<ui_image_t> front_img/*nullptr*/, std::shared_ptr<ui_image_t> back_img/*nullptr*/)
        : ui_base_t(vec3(0.0f))
        , slide_start(0.0f)
        , slide_end(0.0f)
        , percentage(0.0f)
        , slider_button(button)
        , slider_image_front(front_img)
        , slider_image_back(back_img)
    {}

    void ui_slider_t::update(float dt)
    {

    }

    void ui_slider_t::render(vec3 const& _position, mat3 const& _matrix, color_t const& _color)
    {

    }

    bool ui_slider_t::on_event(SDL_Event* event, glm::vec3 const& _position, glm::mat3 const& _matrix)
    {
        return slider_button->on_event(event, _position, _matrix);
    };


    /************************************************************************/
    /* UI List View
    /************************************************************************/
    ui_list_view_t::ui_list_view_t(vec2 position, vec2 size, ui_alignment_e alignment)
    : ui_view_t(position, size, alignment)
    {
    }

    ui_view_t* ui_list_view_t::add_cell(shared_ptr<ui_view_t> cell)
    {
        cell->parent = this;
        // add_child(cell);
        cells.push_back(std::move(cell));
        return cells.back().get();
    }

    void ui_list_view_t::normalize_sizes()
    {
        size_t num_unset = 0;
        float current_size = 0;
        for (auto const& cell : cells)
        {
            float cellsize = (cell->size.x * !vertical) + (cell->size.y * vertical);
            num_unset += cellsize == 0.0f;
            current_size += cellsize;
        }

        float max = (size.x * !vertical) + (size.y * vertical);
        float scale = max / current_size;
        for (auto const& cell : cells) {
            if (vertical)
                cell->size.y *= scale;
            else
                cell->size.x *= scale;

            cell->set_size(cell->size); //updates upper/lower bounds
        }
    }

    void ui_list_view_t::align()
    {
        ui_view_t::align(); //parent align first so that this view is the correct size based on alignment and parent size
        align_cells();
    }

    void ui_list_view_t::align_cells()
    {
        float cur_stride = 0.0;
        for (auto const& cell : cells)
        {
            cell->align();

            //todo: handle non-vertical
            if(vertical)
            {
                cell->position.y = halfsize.y - cell->halfsize.y - cur_stride;
            }
            else
            {
                cell->position.x = -halfsize.x + cell->halfsize.x + cur_stride;
            }
            cur_stride += stride(cell);// cell->size.y + padding;
        }
    }

    void ui_list_view_t::update(float dt)
    {
        ui_view_t::update(dt);

        for(auto& cell : cells)
        {
            cell->update(dt);
        }

        // align_cells();
    }

    void ui_list_view_t::render(vec3 const& _position, mat3 const& _matrix, color_t const& _color)
    {
        DEFAULT_TRANSFORM_COPYPASTA

        // if (color_.a == 0.0f)
        //     return;

        ui_view_t::render(_position, _matrix, _color);
        
        for(auto& cell : cells)
        {
            if(cell->visible)
            {
                cell->render(position_, matrix_, color_);
            }
        }
    }

    bool ui_list_view_t::on_event(SDL_Event* event, glm::vec3 const& _position, glm::mat3 const& _matrix)
    {
        for (auto const& cell : cells)
        {
            if(cell->on_event(event, _position + position, _matrix * matrix))
            {
                return true;
            }
        }

        return ui_view_t::on_event(event, _position, _matrix);
    }

    float ui_list_view_t::stride(std::shared_ptr<ui_view_t> const& cell) const
    {
        return (vertical ? cell->size.y : cell->size.x) + padding;
    }


    /************************************************************************/
    /* UI Table
    /************************************************************************/
    ui_table_t::ui_table_t(glm::vec2(position), glm::vec2(size), ui_alignment_e alignment)
    : ui_list_view_t(position, size, alignment)
    {
    }

    ui_list_view_t* ui_table_t::add_row(float height)
    {
        auto row = add_cell<ui_list_view_t>(vec2(), vec2(0,height), ui_align_horizontal_stretch);
        row->vertical = false;
        row->padding = column_padding;

        rows.push_back(row);

        return row;
    }

    void ui_table_t::set_column_padding(float _padding)
    {
        column_padding = _padding;

        for(auto& row : rows)
        {
            row->padding = column_padding;
        }
    }

    void ui_table_t::align()
    {
        ui_view_t::align();

        //resize each row and col to match row_height an col_width
        if(row_heights.size() > 0)
        {
            size_t row_height_index = 0;
            for(auto& row : rows)
            {
                //row_height_index will cap out at (row_heights.size() - 1)
                row->set_size(row->size.x, row_heights[row_height_index]);

                row_height_index += row_height_index + 1 < (row_heights.size()); //will increment until at the end
            }
        }

        if(col_widths.size() > 0)
        {
            for(auto& row : rows)
            {
                size_t col_width_index = 0;
                for(auto& cell : row->cells)
                {
                    cell->set_size(col_widths[col_width_index], cell->size.y);
                    col_width_index += col_width_index + 1 < (col_widths.size()); //will increment until at the end
                }
            }
        }

        align_cells();
        rows[0]->align_cells();

        ui_list_view_t::align();
    }
}
