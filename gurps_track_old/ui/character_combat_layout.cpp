#include "stdafx.h"
#include "character_combat_layout.h"

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
    character_combat_layout_t::character_combat_layout_t(character_t* character)
    : ui_list_view_t()
    {
        stats              = add_cell<stats_display_t>(        character);
        movement           = add_cell<movement_info_display_t>(character);
        armor              = add_cell<armor_info_display_t>(   character);
    }



    // +-----------------+
    // | ST  8  FT   8/8 |
    // | DX 11           |
    // | IQ 14           |
    // | HT 10  HP 10/10 |
    // +-----------------+
    stat_label_t::stat_label_t(character_t* _character, base_stat_e _stat)
    : labeled_value_t(base_stat_names[_stat], _character->base_stats[_stat])
    , character(_character)
    , stat(_stat)
    {
        btn = add_child<ui_button_t>(vec3(),vec3());
        btn->alignment = ui_align_fill;

        // btn->base_unpressed = label_t::bg;
    }

    stats_display_t::stats_display_t(character_t* character)
    : ui_view_t()
    {
        ASSERT(character, "");

        // /*bg*/add_child<ui_image_t>(chunk_background, vec2(), vec2(), ui_align_fill);

        // ST_label = add_child<stat_label_t>(character, stat_ST);
        // DX_label = add_child<stat_label_t>(character, stat_DX);
        // IQ_label = add_child<stat_label_t>(character, stat_IQ);
        // HT_label = add_child<stat_label_t>(character, stat_HT);

        ST_label = add_child<labeled_value_t>("ST", character->ST());
        DX_label = add_child<labeled_value_t>("DX", character->DX());
        IQ_label = add_child<labeled_value_t>("IQ", character->IQ());
        HT_label = add_child<labeled_value_t>("HT", character->HT());

        fatigue_label = add_child<counting_labeled_value_t>( "FT", character->current_fatigue, character->max_fatigue() );
             HP_label = add_child<counting_labeled_value_t>( "HP", character->current_HP, character->max_HP());
         // basic_speed_label = add_child<labeled_value_t>();
         //        move_label = add_child<labeled_value_t>();


        /// Positioning
        vec2 label_size(140, 52);
        float something = 52.0f;

        float halfheight = (label_size.y / 2.0f) + minor_padding;
        set_size(vec2(label_size.x * 2.0f + something + minor_padding, halfheight * 8.0f));

        auto align_left = [&label_size](labeled_value_t* lbl, float ypos)
        {
            lbl->set_size(label_size);
            lbl->offset.y = ypos;
            lbl->alignment = ui_align_left;
        };

        align_left(ST_label,  halfheight * 3);
        align_left(DX_label,  halfheight);
        align_left(IQ_label, -halfheight);
        align_left(HT_label, -halfheight * 3);


        auto align_right = [&label_size, something](labeled_value_t* lbl, float ypos)
        {
            lbl->set_size(label_size + vec2(something, 0));
            lbl->offset.y = ypos;
            lbl->alignment = ui_align_right;
        };

        align_right(fatigue_label, ST_label->offset.y);
        align_right(HP_label, HT_label->offset.y);
    }

    //  +-----------------+
    //  | Spd: 6.25  Mv 6 |
    //  | Init: X  Enc: H |
    //  +-----------------+
    movement_info_display_t::movement_info_display_t(character_t* character)
    {
        spd_mv = add_cell<ui_view_t>();
        speed    = spd_mv->add_child<labeled_value_t>("Spd", character->basic_speed());
        movement = spd_mv->add_child<labeled_value_t>("Mv",  character->movement());

        init_enc = add_cell<ui_view_t>();
        initiative   = init_enc->add_child<labeled_value_t>("Init", 0);
        encumberance = init_enc->add_child<label_t>("Enc  H");

        auto height = speed->size.y;
        spd_mv->set_size(vec2(100, height));
        init_enc->set_size(vec2(100, height));
    }

    void movement_info_display_t::align()
    {
        ASSERT(parent, "");
        auto width = parent->halfsize3.x - (minor_padding / 2.0f);
        auto height = speed->size.y;

        auto resize_and_align = [&width, &height](ui_view_t* view, ui_alignment_e alignment)
        {
            view->alignment = alignment;
            view->set_size(vec2(width, height));
        };

        resize_and_align(speed,    ui_align_left);
        resize_and_align(movement, ui_align_right);

        resize_and_align(initiative,   ui_align_left);
        resize_and_align(encumberance, ui_align_right);

        // ASSERT(encumberance->children.size() > 0, "");
        // encumberance->children[0]->alignment = ui_align_left;

        ui_list_view_t::align();
    }


    // +-----------------+
    // |   H B A L H F A |
    // |PD 0 2 2 1 2 2   |
    // |DR 0 3 2 1 2 2   |
    // |other pd other dr|
    // +-----------------+
    armor_info_display_t::armor_info_display_t(character_t* character)
    {
        row_heights.push_back(42.0f);
        col_widths. push_back(84.0f);
        col_widths. push_back(42.0f);

        // ASSERT(row_heights.size() > 0, "");
        armor_region_labels = add_row();
        passive_defenses    = add_row();
        damage_resistances  = add_row();

        auto spacer = armor_region_labels->add_cell<ui_view_t>();
        for(size_t i = 1; i < armor_region_count; ++i)
        {
            // armor_region_labels->add_cell<label_t>(armor_region_names[i]);
            auto cell = armor_region_labels->add_cell<label_t>(armor_region_initials[i]);
        }


        passive_defenses->  add_cell<label_t>("PD");
        damage_resistances->add_cell<label_t>("DR");
        for(size_t i = 1; i < armor_region_count; ++i)
        {
            auto pd = passive_defenses->  add_cell<label_t>( to_string(character->PD(armor_region_e(i))) );
            auto dr = damage_resistances->add_cell<label_t>( to_string(character->DR(armor_region_e(i))) );
        }
    }

}
}