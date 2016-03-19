#include "stdafx.h"
#include "character_main_layout.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace gurps_track
{
    // using namespace basic_info;

    // namespace
    // {
        constexpr char info_font[] = "arial";

        constexpr size_t major_padding = 8.0f;
        constexpr size_t minor_padding = 2.0f;
    // }

    ///
    character_main_layout_t::character_main_layout_t(shared_ptr<character_t> _character)
    : ui_list_view_t()
    , character(std::move(_character))
    {
        title_display      = add_cell<title_display_t>(         character.get());
        // character_portrait = add_cell<character_portrait_t>(    character->portrait_filepath);
        // point_cost_sum     = add_cell<point_cost_summary_t>(    character.get());
        experience_log     = add_cell<experience_log_display_t>(character);
    }


    // +-----------------+
    // | Name            |
    // | Race            |
    // | Date Modified   |
    // +-----------------+
    title_display_t::title_display_t(character_t* character)
    {
        ASSERT(character, "");

        padding = minor_padding;

        auto const& date = character->date_modified;

        add_cell<label_t>("Name:"         + character->name);
        auto desc = add_cell<label_t>("Description: " + character->description);
        add_cell<label_t>("Race: "        + character->race.name);
        // add_cell(make_shared<label_t>("#: " + to_string(character->version) + " | " + to_string(date.day) + " / " + to_string(date.month) + " / " + to_string(date.year) ));
        add_cell(make_shared<label_t>("#: " + to_string(character->version) + " | " + date.to_string() ));

        desc->label->text.face_size = 18;
        desc->label->text.alignment = ui_align_left;
    }


    // +-----------------+
    // | Point Costs     |
    // |   ...           |
    // |   ...           |
    // +-----------------+
    constexpr std::array<const char*, point_cost_summary_t::category_count> point_cost_summary_t::category_names;

    point_cost_summary_t::point_cost_summary_t(character_t* character)
    : collapsable_list_view_t("Point Cost Summary", false)
    {
        size.y = 0;
        for(size_t i = 0; i < category_count; ++i)
        {
            auto cell = add_cell<labeled_value_t>(point_cost_summary_t::category_names[i], 0);
            cost_labels[i] = cell;

            cell->alignment = ui_align_horizontal_stretch;
            size.y += cell->size.y + padding;
        }
        size.y -= padding; //make bottom flush
        set_size(size);

        set_data(character);
        // set_expanded(false);
    };

    void point_cost_summary_t::set_data(character_t* character)
    {
        ASSERT(character, "");

        int adv_total = 0;
        int disadv_total = 0;
        int quirk_total = 0;
        for(auto const& trait : character->traits)
        {
            if(trait.is_advantage())
            {
                adv_total += trait.point_cost;
            }
            if(trait.is_disadvantage())
            {
                disadv_total += trait.point_cost;
            }
            if(trait.is_quirk())
            {
                quirk_total += trait.point_cost;
            }
        }


        cost_labels[stats]->        set_data(character->stats_point_cost());
        cost_labels[race]->         set_data(character->race.point_cost());
        cost_labels[advantages]->   set_data(adv_total);
        cost_labels[disadvantages]->set_data(disadv_total);
        cost_labels[quirks]->       set_data(quirk_total);
        cost_labels[skills]->       set_data(static_cast<int>(character->skills_point_cost()));
        cost_labels[spells]->       set_data(character->spells_point_cost());
        //lazy, should save totals I grabbed before to the stack and sum them here rather than re-loop
        cost_labels[total]->        set_data(character->total_point_cost()); 
    }

    // +-----------------+
    // |- EXP Info Log  -|
    // | +4   Nov 20     |
    // | -2   Lev 17->20 |
    // |-0.5  Alchemy    |
    // |  ...            |
    // | Unspent Pts: ## |  //does not collapse
    experience_log_display_t::experience_log_display_t(std::shared_ptr<character_t> _character)
    : collapsable_list_view_t("EXP Info Log")
    , character(move(_character))
    {
        for(auto const& transaction : character->point_transactions)
        {
            std::string str = " " + to_string(transaction.amount);

            if(transaction.amount > 0)
                str[0] = '+';
            else if(transaction.amount < 0)
                str[0] = '-';

            auto cell = add_cell<labeled_value_t>(str, transaction.date.to_string());
            cell->size.y = 50;
        }

        unspent_points = add_child<labeled_value_t>("Unspent: ", character->unspent_points);
        unspent_points->alignment = ui_align_bottom;

        increment_points_btn = unspent_points->add_child<button_view_t>( "+", [this](ui_base_t*){ this->increment_unspent_points(); } );
        decrement_points_btn = unspent_points->add_child<button_view_t>( "-", [this](ui_base_t*){ this->increment_unspent_points(); } );

        increment_points_btn->alignment = ui_align_right;
        increment_points_btn->set_size(40,40);
        increment_points_btn->offset.x = -40.0f - minor_padding;

        decrement_points_btn->alignment = ui_align_right;
        decrement_points_btn->set_size(40,40);        
    }

    void experience_log_display_t::align()
    {
        collapsable_list_view_t::align();

        // set_size(size += vec2(0, unspent_points->size.y + minor_padding));
        // set_size(500, 200);
        unspent_points->position.y += unspent_points->size.y + minor_padding;
    }

    void experience_log_display_t::increment_unspent_points()
    {
        // todo: implement a UI for incrementing by a specific amount with a memo
        
        character->points_transaction(1, "das increment");
    }

    void experience_log_display_t::decrement_unspent_points()
    {
        character->points_transaction(1, "das un-increment");
    }



    // +-----------------+
    // |                 |
    // |    *portrait*   |
    // |                 |
    // |                 |
    // |    *portrait*   |
    // |                 |
    // +-----------------+
    character_portrait_t::character_portrait_t(std::string const& filepath)
    : ui_view_t(vec2(), vec2(200.0f, 260.0f))
    {
        auto container = add_child<ui_view_t>();
        container->alignment = ui_align_fill;

        if(filepath.size() > 0)
        {
            try
            {
                auto portrait_texture = make_shared<texture_t>(filepath);
                portrait = add_child<ui_image_t>(std::move(portrait_texture), vec2());
            }
            catch (content_load_exception const& e)
            {
                portrait = add_child<ui_image_t>( COLOR_WHITE, vec2(), size );
            }
        }
        else
        {
            portrait = add_child<ui_image_t>( COLOR_WHITE, vec2(), size );
        }

        portrait->alignment = ui_align_center;
    }

}
}