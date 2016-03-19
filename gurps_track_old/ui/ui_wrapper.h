#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"

#include "stdafx.h"
#include "ui_base.h"
#include "ui_button.h"
#include "ui_label.h"

namespace asdf
{
namespace gurps_track
{
namespace ui
{
    namespace layout_constants
    {
        const color_t background_color = COLOR_DARKGREY;
        constexpr char info_font[] = "arial";

        const color_t chunk_background = COLOR_BLACK;

        const color_t labeled_value_bg_color = COLOR_GREY;
        constexpr size_t stat_font_size = 20;

        constexpr size_t major_padding = 8.0f;
        constexpr size_t minor_padding = 2.0f;
        const glm::vec2 margin{6.0f, 2.0f};

        const color_t button_unpressed_color = glm::vec4(0.4, 0.4, 0.8, 1.0);
        const color_t button_highlight_color = button_unpressed_color + glm::vec4(0.2, 0.2, 0.2, 0.0);
        const color_t button_pressed_color   = button_unpressed_color + glm::vec4(0.1, 0.4, 0.2, 0.0);
        const color_t button_disabled_color  = glm::vec4(0.2, 0.2, 0.2, 1.2);

        const color_t expanded_img_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        const color_t collapsed_img_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

    struct label_t : ui_view_t
    {
        ui_image_t* bg;
        ui_label_t* label;

        label_t(std::string const&);
        void set_text(std::string str);
    };

    struct labeled_value_t : label_t
    {
        ui_label_t* value_label;

        labeled_value_t(std::string const&, std::string const&);
        labeled_value_t(std::string const&, const int value);
        void set_data(const int value);
    };

    struct counting_labeled_value_t : labeled_value_t
    {
        counting_labeled_value_t(std::string const&, const int value1, const int value2);
        void set_data(const int value1, const int value2);
    };

    struct button_view_t : ui_view_t
    {
        ui_button_t* btn;
        ui_label_t* label;

        button_view_t(ui_function_t on_end_inside_func);
        button_view_t(std::string const& label_str, ui_function_t on_end_inside_func);

        void set_text(std::string str);
    };

    // template <size_t Num_Cells>
    // struct ui_split_view_ : ui_view_t
    // {
    //     std::array<ui_view_t*, Num_Cells> cells;

    //     ui_split_view_t(glm::vec2 const& _position = vec2(), glm::vec2 const& _size = vec2(), ui_alignment_e _alignment = ui_align_center)
    //     : : ui_view_t(_position, _size, _alignment)
    //     {
    //         for(size_t i = 0; i < Num_Cells; ++i)
    //         {
    //             cells[i] = add_child<ui_view_t>();
    //         }
    //     }

    //     virtual void align() override
    //     {

    //     }
    // };

    // using ui_split_view_t = ui_split_view_<2>;

    struct ui_list_view_t : asdf::ui_list_view_t
    {
        ui_list_view_t();

        virtual void align() override;
    };

    struct collapsable_list_view_t : ui_list_view_t
    {
        button_view_t* button = nullptr;
        ui_image_t* expanded_img = nullptr;
        ui_image_t* collapsed_img = nullptr;

        std::string name;
        float _height = 0.0f; //re-expand to this height when re-expanding

        collapsable_list_view_t(std::string name, bool init_expanded = true);

        // virtual void align() override;

        void set_expanded(bool expanded);
        bool is_expanded() const;
    };

    struct ui_table_t : asdf::ui_table_t
    {
        ui_table_t();

        virtual void align() override;
    };

    /// Struct containing some number of context-useful buttons
    struct menu_popup_t
    {

    };

    /// Contains plus and minus buttons
    struct label_mod_popup_t : ui_view_t
    {
        button_view_t* plus = nullptr;
        button_view_t* minus = nullptr;

        label_mod_popup_t(int& mod);
    };
}
}

#pragma clang diagnostic pop
