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
    /*
    +--------------------+
    | Name               |
    | Race               |
    | Date Modified      |
    | *User Defined*     |
    |   ...              |
    +--------------------+
    |                    |
    |    *portrait*      |
    |                    |
    |                    |
    |    *portrait*      |
    |                    |
    +--------------------+
    |-   Point Costs    -|
    |   ...              |
    |   ...              |
    +--------------------+
    |-   EXP Info Log   -|
    | +4   Nov 20        |
    | -2   Lev 17->20    |
    |-0.5  Alchemy       |
    |  ...               |
    |Unspent Pts: ##  [+][-]|  //does not collapse
    +---------------------+
    */

    struct title_display_t : ui_list_view_t
    {
        title_display_t(character_t* character);
    };

    struct point_cost_summary_t : collapsable_list_view_t
    {
        enum cost_categories_e
        {
              stats = 0
            , race
            , advantages
            , disadvantages
            , quirks
            , skills
            , spells
            , total
            , category_count
        };

        static constexpr std::array<const char*, point_cost_summary_t::category_count> category_names =
        {
              "Stats"
            , "Race"
            , "Advantages"
            , "Disadvantages"
            , "Quirks"
            , "Skills"
            , "Spells"
            , "Total"
        };

        std::array<labeled_value_t*, category_count> cost_labels;

        point_cost_summary_t(character_t* character);
        void set_data(character_t* character);
    };

    struct experience_log_display_t : collapsable_list_view_t
    {
        labeled_value_t* unspent_points = nullptr;
        button_view_t* increment_points_btn = nullptr;
        button_view_t* decrement_points_btn = nullptr;

        std::shared_ptr<character_t> character;

        experience_log_display_t(std::shared_ptr<character_t> _character);

        void align() final;

        void increment_unspent_points();
        void decrement_unspent_points();
    };

    struct character_portrait_t : ui_view_t
    {
        ui_image_t* portrait = nullptr;
        character_portrait_t(std::string const& filepath);
    };


    struct character_main_layout_t : ui_list_view_t
    {
        title_display_t*          title_display      = nullptr;
        point_cost_summary_t*     point_cost_sum     = nullptr;
        character_portrait_t*     character_portrait = nullptr;
        experience_log_display_t* experience_log     = nullptr;

        std::shared_ptr<character_t> character;

        character_main_layout_t(std::shared_ptr<character_t> _character);
    };
}
}

#pragma clang diagnostic pop
