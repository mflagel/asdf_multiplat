#include "stdafx.h"
#include "ui_wrapper.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace gurps_track
{
    using namespace ui::layout_constants;

    // using namespace basic_info;

    // namespace
    // {
        
    // }

    label_t::label_t(std::string const& str)
    : ui_view_t()
    {
        bg = add_child<ui_image_t>(labeled_value_bg_color, vec2(), vec2(), ui_align_fill);
        label = add_child<ui_label_t>(str, Content.fonts[info_font]);

        label->alignment = ui_align_fill;

        set_size(label->size + vec2(major_padding * 2));
    }

    void label_t::set_text(std::string str)
    {
        label->set_text(std::move(str));
    }

    labeled_value_t::labeled_value_t(std::string const& _str, std::string const& _str2)
    : label_t(_str)
    {
        value_label = add_child<ui_label_t>(_str2, Content.fonts[info_font]);

        label->alignment = ui_align_left;
        value_label->alignment = ui_align_right;

        label->offset.x += margin.x;
        value_label->offset.x -= margin.x;
    }

    labeled_value_t::labeled_value_t(std::string const& _str, const int value)
    : labeled_value_t(_str, to_string(value))
    {
    }

    void labeled_value_t::set_data(const int value)
    {
        ASSERT(value_label, "");
        value_label->set_text(to_string(value));
    }

    counting_labeled_value_t::counting_labeled_value_t(std::string const& _str, const int value1, const int value2)
    : labeled_value_t(_str, 0)
    {
        auto _width = value_label->size.x;
        set_data(value1, value2);

        set_size(vec2(size.x + (value_label->size.x - _width), size.y));
    }

    void counting_labeled_value_t::set_data(const int value1, const int value2)
    {
        value_label->set_text(to_string(value1) + "/" + to_string(value2));
    }

    button_view_t::button_view_t(ui_function_t on_end_inside_callback)
    {
        btn = add_child<ui_button_t>(vec3(), vec3());
        btn->alignment = ui_align_fill;
        btn->on_end_inside_callback = std::move(on_end_inside_callback);

        btn->base_unpressed = make_shared<ui_image_t>(button_unpressed_color, vec2(), vec2(), ui_align_fill);
        btn->base_highlight = make_shared<ui_image_t>(button_highlight_color, vec2(), vec2(), ui_align_fill);
        btn->base_pressed   = make_shared<ui_image_t>(button_pressed_color,   vec2(), vec2(), ui_align_fill);
        btn->base_disabled  = make_shared<ui_image_t>(button_disabled_color,  vec2(), vec2(), ui_align_fill);

        btn->add_child(btn->base_unpressed);
        btn->add_child(btn->base_highlight);
        btn->add_child(btn->base_pressed);
        btn->add_child(btn->base_disabled);
    }

    button_view_t::button_view_t(std::string const& label_str, ui_function_t on_end_inside_callback)
    : button_view_t(std::move(on_end_inside_callback))
    {
        label = add_child<ui_label_t>(label_str, Content.fonts[info_font]);
    }

    void button_view_t::set_text(std::string str)
    {
        label->set_text(std::move(str));
    }

    ui_list_view_t::ui_list_view_t()
    : asdf::ui_list_view_t()
    {
        padding = minor_padding;

        // /*bg*/add_child<ui_image_t>(chunk_background, vec2(), vec2(), ui_align_fill);
    }

    void ui_list_view_t::align()
    {
        float total_stride = 0.0f;
        for(auto const& cell : cells)
        {
            cell->alignment = vertical ? ui_align_horizontal_stretch : ui_align_vertical_stretch;
            if(cell->visible)
            {
                total_stride += stride(cell);
            }
        }
        total_stride -= padding; //make bottom flush

        if(vertical)
        {
            set_size(vec2(size.x, total_stride));
        }
        else
        {
            set_size(vec2(total_stride, size.y));
        }

        asdf::ui_list_view_t::align();
    }

    collapsable_list_view_t::collapsable_list_view_t(std::string _name, bool init_expanded)
    : ui_list_view_t()
    , name(std::move(_name))
    {
        button = add_cell<button_view_t>(name, [this](ui_base_t* sender){ this->set_expanded(button->btn->toggled); });
        button->alignment = ui_align_top_stretch;
        // button->alignment_edge = ui_align_edge_outside;
        button->btn->togglable = true;
        button->btn->toggled = true;
        button->set_size(vec2(100, 50));

         expanded_img = button->add_child<ui_image_t>( expanded_img_color, vec2(), vec2(20.0f), ui_align_right);
        collapsed_img = button->add_child<ui_image_t>(collapsed_img_color, vec2(), vec2(20.0f), ui_align_right);

         expanded_img->offset.x -= major_padding;
        collapsed_img->offset.x -= major_padding;

        set_expanded(init_expanded);
    }

    void collapsable_list_view_t::set_expanded(const bool expanded)
    {
         expanded_img->visible =  expanded;
        collapsed_img->visible = !expanded;

        for(auto& cell : cells)
        {
            cell->visible = expanded;
        }

        button->visible = true; //button is a cell, so override vis to true
        // button->toggled = expanded;

        if(expanded)
        {
            //todo: get proper size based on sum of children heights
            //      otherwise things might break if cells are added while un-expanded
            set_size(vec2(size.x, _height));
        }
        else
        {
            _height = size.y;
            set_size(vec2(size.x, button->size.y));
        }

        top_parent()->align(); //realign everything
    }

    bool collapsable_list_view_t::is_expanded() const
    {
        return expanded_img->visible;
    }

    ui_table_t::ui_table_t()
    : asdf::ui_table_t()
    {
        padding = minor_padding;
        column_padding = minor_padding;
    }

    void ui_table_t::align()
    {
        for(auto& row : rows)
        {
            row->alignment = ui_align_horizontal_stretch;

            for(auto& cell : row->cells)
            {
                cell->alignment = ui_align_vertical_stretch;
            }
        }

        asdf::ui_table_t::align();
    }

    label_mod_popup_t::label_mod_popup_t(int& mod)
    : ui_view_t(vec2(), vec2(62.0f, 22.0f))
    {
        plus  = add_child<button_view_t>("+", [&mod](ui_base_t*){ ++mod; });
        minus = add_child<button_view_t>("-", [&mod](ui_base_t*){ --mod; });
    }

}
}
