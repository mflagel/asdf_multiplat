#include "stdafx.h"

#include "gurps_track.h"
#include "test_layout.h"
#include "content_manager.h"

using namespace std;
using namespace glm;

namespace asdf
{
    using namespace gurps_track;

    test_layout_t::test_layout_t()
        : ui_view_t(glm::vec2(0.0f), glm::vec2(640.0f, 480.0f), ui_align_center)
    {
        Content.fonts["arial"]->FaceSize(14);

        auto test_view_bg = make_shared<ui_image_t>(COLOR_DARKGREY, glm::vec2(), glm::vec2(), ui_align_fill);
        auto test_view_title = make_shared<ui_image_t>(COLOR_GREY, glm::vec2(0, -15), glm::vec2(0, 30), ui_align_top_stretch);
            auto test_view_label = make_shared<ui_label_t>("Whoamagad A Title", Content.fonts["arial"], glm::vec2(), ui_align_center);
                test_view_label->add_child(make_shared<ui_image_t>(COLOR_RED, glm::vec2(), glm::vec2(5, 5), ui_align_center));
            test_view_title->add_child(test_view_label);

        auto test_subview = make_shared<ui_view_t>(glm::vec2(0, 25), glm::vec2(50, 50), ui_align_bottom_stretch);
            test_subview->add_child(make_shared<ui_image_t>(COLOR_LIGHTGREY, glm::vec2(), glm::vec2(), ui_align_fill));

            //test_view->add_child(make_shared<ui_label_t>("Testing~", Content.fonts["arial"], glm::vec2(), ui_align_none));

        auto test_button = make_shared<test_button_t>(glm::vec3(-280, 0, 0), glm::vec3(50, 20, 0));
            test_button->add_child(make_shared<ui_label_t>("Play", Content.fonts["arial"], glm::vec2(), ui_align_center));

        debug_views_btn = make_shared<test_button_t>(glm::vec3(-200, 200, 0), glm::vec3(100, 20, 0), true);
            debug_views_btn->add_child(make_shared<ui_label_t>("Debug Views", Content.fonts["arial"], glm::vec2(), ui_align_center));

            debug_views_btn->on_end_inside_callback = [this](ui_base_t* psender){
                app.render_debug_views = debug_views_btn->state == ui_button_t::down;
            };

        debug_views_btn->state = ui_button_t::down;

        auto test_list_view = make_shared<ui_list_view_t>(vec2(-200, 0), vec2(50, 200), ui_align_center);
            test_list_view->vertical = true;
            test_list_view->add_child(make_shared<ui_image_t>(color_t(0.6, 0.6, 0.6, 1.0f)));

            auto test_cell = make_shared<ui_view_t>(vec2(), vec2(50,40));
                test_cell->add_child(make_shared<ui_label_t>("test", Content.fonts["arial"], glm::vec2(), ui_align_center));
            auto test_cell2 = make_shared<ui_view_t>(vec2(), vec2(50,40));
                test_cell2->add_child(make_shared<ui_label_t>("test2", Content.fonts["arial"], glm::vec2(), ui_align_center));

            test_list_view->cells.push_back(std::move(test_cell));
            test_list_view->cells.push_back(std::move(test_cell2));

            test_list_view->cells.push_back(std::move(make_shared<ui_label_t>("test3", Content.fonts["arial"], glm::vec2(), ui_align_center)));
            test_list_view->cells.push_back(std::move(make_shared<ui_label_t>("test4", Content.fonts["arial"], glm::vec2(), ui_align_center)));
            test_list_view->cells.push_back(std::move(make_shared<ui_label_t>("test5", Content.fonts["arial"], glm::vec2(), ui_align_center)));
        


        test_table = make_shared<ui_table_t>(vec2(-160, 0), vec2(200,300), /**/ui_align_right/**/);
        test_table->add_child(make_shared<ui_image_t>(color_t(0.4, 0.4, 0.6, 1.0f)));

        for (size_t i = 0; i < 3; ++i) {
            // auto temp = make_shared<ui_view_t>(vec2(), vec2(300, 40), ui_align_center);
            //auto row = make_shared<ui_list_view_t>(vec2(), vec2(300,40));
            auto row = test_table->add_row(40);

            for (size_t j = 0; j < 1; j++) {
               shared_ptr<ui_view_t> cell = make_shared<ui_view_t>(vec2(), vec2(80.0f, 40.0f));

               cell->add_child(make_shared<ui_label_t>(to_string(i) + ", " + to_string(j), Content.fonts["arial"], glm::vec2(), ui_align_center));

               row->add_cell(std::move(cell));
            }

            // row->normalize_sizes();
            // test_table->rows->add_child(temp);
            // test_table->rows->add_cell(std::move(row));
        }


        add_child(test_view_bg);
        add_child(test_view_title);
        add_child(test_subview);

        // add_child(test_button);
        add_child(debug_views_btn);

        // add_child(test_list_view);
        // add_child(test_table);
    }

    test_layout_t::test_button_t:: test_button_t(glm::vec3 pos, glm::vec3 size, bool togglable)
        : ui_button_t(pos, size, togglable)
    {
        base_unpressed = make_shared<ui_image_t>(COLOR_BLUE);
        base_highlight = make_shared<ui_image_t>(glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
        base_pressed   = make_shared<ui_image_t>(glm::vec4(0.1f, 0.1f, 0.5f, 1.0f));
        base_disabled  = make_shared<ui_image_t>(glm::vec4(0.0f, 0.0f, 0.1f, 1.0f));
    }
}