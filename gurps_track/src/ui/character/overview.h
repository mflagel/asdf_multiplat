#pragma once

#include <kiui/Ui/mkUiConfig.h>
#include <kiui/Ui/mkUiTypes.h>

namespace gurps_track
{

namespace data
{
    struct character_t;
}

namespace ui
{
namespace character
{
    /***  Overview ***
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


    struct title_display_t : mk::Expandbox
    {
        title_display_t(data::character_t&);
    };

    struct portrait_t : mk::Expandbox
    {
        portrait_t(data::character_t&);
    };

    struct point_cost_summary_t : mk::Expandbox
    {
        enum cost_categories_e
        {
              stats = 0
            // , race
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
            // , "Race"
            , "Advantages"
            , "Disadvantages"
            , "Quirks"
            , "Skills"
            , "Spells"
            , "Total"
        };

        std::array<mk::Label*, category_count> cost_labels;

        point_cost_summary_t(data::character_t&);
        void set_data(data::character_t&);
    };

    struct experience_log_t : mk::Expandbox
    {
        experience_log_t(data::character_t&);
    };

    struct labeled_textbox_t : mk::Sequence
    {
        mk::Label& label;
        mk::Textbox& textbox;

        labeled_textbox_t(std::string const& label_str, std::string const& textbox_str);
    };

    /**
    **/
    struct overview_t : mk::Sheet
    {
        mk::labeled_textbox_t* name        = nullptr;
        mk::labeled_textbox_t* description = nullptr;
        mk::labeled_textbox_t* race        = nullptr;
        mk::Label* date_modified = nullptr;

        overview_t(data::character_t&);
    };

}
}
}