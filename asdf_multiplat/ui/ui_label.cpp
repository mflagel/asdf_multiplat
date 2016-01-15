#include "stdafx.h"
#include "ui_label.h"
#include "utilities.h"

using namespace glm;
using namespace std;
using namespace asdf::util;

namespace asdf
{
    ui_label_t::ui_label_t(string str, FTFont* font, glm::vec2 _position, ui_alignment_e alignment/*ui_align_none*/)
    : ui_view_t(_position, vec2(), alignment)
    {
        text.font = font;

        set_text(std::move(str));
    }

    ui_label_t::ui_label_t(string str, FTFont* font, size_t face_size, ui_alignment_e text_alignment)
    : ui_view_t()
    {
        text.font = font;
        text.face_size = face_size;
        text.alignment = text.alignment;
        
        set_text(std::move(str));
    }

    void ui_label_t::resize_to_text()
    {
        auto bounding_box = text.font->BBox(str().c_str()); //just grabing width/height
        size.x = abs(bounding_box.Upper().Xf() - bounding_box.Lower().Xf());
        size.y = abs(bounding_box.Upper().Yf() - bounding_box.Lower().Yf());
        halfsize = size / 2.0f;
        upper_bound = position + vec3(halfsize, 0);
        lower_bound = position - vec3(halfsize, 0);
    }

    void ui_label_t::set_text(std::string _str)
    {
        text.str = move(_str);
        resize_to_text();
    }

    void ui_label_t::align()
    {
        ui_view_t::align();

        text.align(halfsize);
    }

    void ui_label_t::update(float dt)
    {
        ui_view_t::update(dt);
    }

    void ui_label_t::render(vec3 const& _position, mat3 const& _matrix, color_t const& _color)
    {
        //todo: convert draw pos to handle multiple resolutions
        vec3 position_ = _position + position;
        mat3 matrix_ = _matrix * matrix;
        color_t color_ = _color * color;

        app.spritebatch->draw_text(str(), text.font, text.face_size, position_.xy + text.offset, color * _color);

        ui_view_t::render(_position, _matrix, _color);
    }

    void ui_label_t::render_debug()
    {
        ui_base_t::render_debug();

        vec3 hsize = vec3(text.halfsize, 0.0f);
        vec3 pos = get_world_position() + vec3(text.offset + text.halfsize, 0.0f);

        //lazy debug draw
        glBegin(GL_POLYGON);
        glVertex2f(pos.x - hsize.x, pos.y + hsize.y);
        glVertex2f(pos.x + hsize.x, pos.y + hsize.y);
        glVertex2f(pos.x + hsize.x, pos.y - hsize.y);
        glVertex2f(pos.x - hsize.x, pos.y - hsize.y);
        glEnd();
    }
}