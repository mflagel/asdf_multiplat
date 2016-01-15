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
    struct character_skill_library_t;
    
    /*
    +---------------------+
    |-Skill Name (0.5) 12-|
    |  (P/E) 11 + 1 [+][-]|
    |  +1 Combat Reflexes |
    |  Description        |
    +---------------------+
     */
    struct character_skill_library_entry_t : collapsable_list_view_t
    {
        // button_view_t* remove_button;

        ui_label_t* skill_name = nullptr;
        ui_label_t* point_cost = nullptr;
        ui_label_t* effective_skill = nullptr;

        label_t* extra_info = nullptr;
        button_view_t* increment_button = nullptr;
        button_view_t* decrement_button = nullptr;


        label_t* description = nullptr;

        character_skill_library_t* library = nullptr;
        size_t index = 90001;

        character_skill_library_entry_t(character_skill_library_t* library, size_t index);

        void set_data(skill_listing_t const&);

        void change_point_value(bool positive);
        void increment_point_value();
        void decrement_point_value();
    };

    /*
    +-----------------+
    |- Phsysical        -|
    | Name   2  13 (P/E) |
    |-Name  0.5 12       |
    |  11 + 1(bonus desc)|
    | Name   8  15 (P/E) |
    |-Name   0  13 (P/E) |
    |   Default: X       |
    | Name   4  12 (P/H) |
    |                    |
    |- Mental           -|
    |  Lang 0 R 15 (M/A) |
    |  Lang 0.5 14 (M/A) |
    +-----------------+
    */
    struct character_skill_library_t : ui_list_view_t
    {
        collapsable_list_view_t* physical_skills;
        collapsable_list_view_t* mental_skills;

        size_t prev_skill_count = 0;
        std::shared_ptr<character_t> character;

        character_skill_library_t(std::shared_ptr<character_t> _character);

        void rebuild_list();

        void update(float dt) final;
    };


/*
    +-----------------+
    |     Skills      |
    +-----------------+
    |- Phsysical       -|
    | Name   2  13 (P/E)|
    |  ...              |
    |  
    |- Mental          -|
    |  Lang 0 R 15 (M/A)|
    |  Skill 1 16  (M/A)|
    |   ...             |
    +-----------------+
    */    
    struct character_skills_layout_t : ui_list_view_t
    {
        std::shared_ptr<character_t> character;

        character_skill_library_t* skill_library;

        character_skills_layout_t(std::shared_ptr<character_t> _character);
    };
}
}

#pragma clang diagnostic pop
