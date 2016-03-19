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
    +-----------------+
    | ST  8  FT  8/8  |
    | DX 11           |
    | IQ 14           |
    | HT 10  HP 10/10 |
    +-----------------+
    | Spd: 6.25  Mv 6 |
    | Init: X  Enc: H |
    +-----------------+
    |   H B A L H F A |
    |PD 0 2 2 1 2 2   |
    |DR 0 3 2 1 2 2   |
    |other pd other dr|
    +-----------------+
    |-    Damage     -|
    |Thr: 1d-1  Sw:1d |
    |Kick: 1d-1       |
    +-----------------+
    |-    Attacks    -|
    |- Large Knife   -|
    |   Cut           |
    |    {cut stats}  |
    |   Imp           |
    |    {imp stats}  |
    |- Small Knife   -|
    |- Dagger        -|
    |                 |
    |- Poltergeist   -|
    |   {ranged stats}|
    |                 |
    |- Ammunition    -|
    +-----------------+
    |-   Defenses    -|
    | Dodge: 6 + PD   |
    |-Parry: 7 + PD  -|  // default collapsed
    |  Weapon 1 Parry |
    |  Weapon 2 Parry |
    |  Weapon 3 Parry |
    | Block: 5 + PD   |
    +-----------------+
    |- Status Effects-|
    | Poisoned        |
    |   HP -= 2/s     |
    | Deafened        |
    +-----------------+
    |- Active Spells -|
    |  Penalty: -##   |
    |-  Illusions ## -|
    |- Simple  2      |
    |    Description  |
    |      +5  +3A    |  // spell pass by 5, art pass by 3
    |    Description  |
    |      +1  -4A    |
    |- Complex  1     |
    |    Description  |
    |      +2  +2A    |  // spell pass by 5, art pass by 3
    |- Keen Eyes 3    |
    |    Description  |  // ex: when spell was casted
    |      +2         |
    |- Levitation     |
    |    Weight: 90lb |
    |    +10          |
    +-----------------+
    */

    struct stat_label_t : labeled_value_t
    {
        character_t* character = nullptr;
        base_stat_e stat;

        ui_button_t* btn = nullptr;

        stat_label_t(character_t*, base_stat_e);
    };

    struct stats_display_t : ui_view_t
    {
        labeled_value_t* ST_label = nullptr;
        labeled_value_t* DX_label = nullptr;
        labeled_value_t* IQ_label = nullptr;
        labeled_value_t* HT_label = nullptr;

        counting_labeled_value_t* fatigue_label = nullptr;
        counting_labeled_value_t* HP_label = nullptr;

        labeled_value_t* basic_speed_label = nullptr;
        labeled_value_t* move_label = nullptr;

        stats_display_t(character_t* character);
    };

    struct movement_info_display_t : ui_list_view_t
    {
        ui_view_t* spd_mv;
        ui_view_t* init_enc;

        labeled_value_t* speed = nullptr;
        labeled_value_t* movement = nullptr;

        labeled_value_t* initiative = nullptr;
        label_t* encumberance = nullptr;

        movement_info_display_t(character_t* character);

        virtual void align() final;
    };

    struct armor_info_display_t : ui_table_t
    {
        ui_list_view_t* armor_region_labels = nullptr;
        ui_list_view_t* passive_defenses    = nullptr;
        ui_list_view_t* damage_resistances  = nullptr;

        armor_info_display_t(character_t* character);
    };

    struct status_effects_t : ui_list_view_t
    {

    };

    struct character_combat_layout_t : ui_list_view_t
    {
        stats_display_t*         stats = nullptr;
        movement_info_display_t* movement = nullptr;
        armor_info_display_t*    armor = nullptr;

        character_combat_layout_t(character_t* character);
    };
}
}

#pragma clang diagnostic pop
