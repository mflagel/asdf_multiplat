#pragma once

#include "stdafx.h"
#include "ui_base.h"

namespace asdf {

    struct test_layout_t : ui_view_t
    {
        struct test_button_t : ui_button_t
        {
            test_button_t(glm::vec3 pos, glm::vec3 size, bool togglable = false);
        };

        std::shared_ptr<ui_button_t> debug_views_btn;
        std::shared_ptr<ui_table_t> test_table;

        test_layout_t();
    };
}