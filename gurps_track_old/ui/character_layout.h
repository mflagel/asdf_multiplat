#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"

#include "stdafx.h"
#include "character.h"
#include "ui_wrapper.h"

namespace asdf
{
namespace gurps_track
{

    struct character_main_layout_t;
    struct character_combat_layout_t;
    struct character_traits_layout_t;
    struct character_skills_layout_t;
    struct character_spells_layout_t;


    /**  Pretty much each grouping has its own struct   **
    +-----------------+
    |New|Load|Save|Save As|
    +-----------------+
    |*M*| C | T | Sk | Sp |
    +-----------------+
    |                 |
    | *Tab Specific*  |
    |                 |
    |                 |
    |                 |
    | *Tab Specific*  |
    |                 |
    |        .        |
    |        .        |
    |        .        |
    +-----------------+
    */

    struct character_layout_t : ui_view_t
    {
        enum layout_tab_e
        {
              tab_main
            , tab_combat
            , tab_traits
            , tab_skills
            , tab_spells
            , tab_count
        };

        std::shared_ptr<character_t> character;

        ui_list_view_t* tabs_container = nullptr;
        std::array<button_view_t*, tab_count> tab_buttons;

        ui_view_t*           container = nullptr;

        character_main_layout_t*   main   = nullptr;
        character_combat_layout_t* combat = nullptr;
        character_traits_layout_t* traits = nullptr;
        character_skills_layout_t* skills = nullptr;
        character_spells_layout_t* spells = nullptr;

        std::array<ui_view_t*, tab_count> tab_pages;

        character_layout_t(std::shared_ptr<character_t> _character);

        virtual void align() final;

        void set_tab(layout_tab_e);

        void new_file_callback();
        void open_file_callback();
        void save_file_callback();
        void save_as_file_callback();
    };
}
}

#pragma clang diagnostic pop
