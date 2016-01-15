#pragma once;

#include "stdafx.h"
#include <FTGL/ftgl.h>

#include "ui_base.h"

namespace asdf
{
    struct text_info_t
    {
        std::string str;
        FTFont* font = nullptr;
        size_t face_size = 32;
        ui_alignment_e alignment = ui_align_center;

        glm::vec2 offset;
        glm::vec2 halfsize;   

        void align(glm::vec2 parent_halfsize);
    };
}
