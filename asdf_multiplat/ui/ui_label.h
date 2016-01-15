#pragma once;

#include "stdafx.h"
#include "text.h"

#include "ui_base.h"

namespace asdf
{
    struct ui_label_t : ui_view_t
    {   
        text_info_t text;

        //ui_label_t(std::shared_ptr<ui_base_t> parent, std::string const& str, std::shared_ptr<FTFont> font, glm::vec2 position);
        ui_label_t(std::string, FTFont* font, glm::vec2 position = glm::vec2(0), ui_alignment_e alignment = ui_align_center);
        ui_label_t(std::string, FTFont* font, size_t face_size, ui_alignment_e text_alignment);

        void resize_to_text();
        void set_text(std::string str);

        std::string const& str() const { return text.str; }

        void align() override;

        virtual void update(float dt) override;
        virtual void render( UI_RENDER_PARAMS) override;
        virtual void render_debug() override;
    };
}