#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wc++1y-extensions" //will remove this when I upgrade my clang and switch to c++14

#include "stdafx.h"
#include "asdfm/utilities/str_const.h"

namespace gurps_track
{
namespace data
{
// namespace basic_info
// {
    /*************************************************/
    /*** BASE STATS INFO
    /*************************************************/
    enum base_stat_e
    {
          stat_ST = 0
        , stat_DX
        , stat_IQ
        , stat_HT
        , base_stat_count
    };

    enum derived_stat_e
    {
          stat_HP          = 0
        , stat_fatigue     = 1
        , stat_speed       = 2
        , stat_movement    = 3
        , stat_initiative  = 4
        , stat_willpower   = 5
        , stat_vision      = 6
        , stat_hearing     = 7
        , stat_smell_taste = 8
        , derived_stat_count
    };

    enum special_stat_e
    {
          stat_magery
        , stat_clerical_investment
        , special_stat_count
    };

    constexpr size_t total_stat_count = base_stat_count + derived_stat_count + special_stat_count;

    constexpr size_t sense_stat_start_index = stat_vision;
    constexpr size_t sense_stat_end_index = stat_smell_taste;

    constexpr std::array<const char*, base_stat_count> base_stat_names =
    {
          "ST"
        , "DX"
        , "IQ"
        , "HT"
    };

    constexpr std::array<const char*, derived_stat_count> derived_stat_names = 
    {
          "HP"
        , "Fatigue"
        , "Speed"
        , "Movement"
        , "Initiative"
        , "Willpower"
        , "Vision"
        , "Hearing"
        , "Taste/Smell"
    };

    constexpr std::array<const char*, derived_stat_count> special_stat_names = 
    {
          "Magery"
        , "Clerical Investment"
    };

    constexpr std::array<int, 21> stat_point_costs = 
    {
          90001 //stat value of 0 is not possible
        , -80   // 1
        , -70   // 2
        , -60   // 3
        , -50   // 4
        , -40   // 5
        , -30   // 6
        , -20   // 7
        , -15   // 8
        , -10   // 9
        ,  0    // 10
        ,  10   // 11
        ,  20   // 12
        ,  30   // 13
        ,  45   // 14
        ,  60   // 15
        ,  80   // 16
        , 100   // 17
        , 125   // 18
        , 150   // 19
        , 175   // 20
    };

    
    constexpr int get_stat_point_cost(size_t level)
	{
		return stat_point_costs[level > 20 ? 20 : level] + 25 * (20 - (level > 20 ? level : 20));
	}


    constexpr int get_stat_point_improvement_cost(size_t from_level, size_t to_level)
    {
        return get_stat_point_cost(to_level) - get_stat_point_cost(from_level);
    }

    constexpr int get_stat_point_improvement_cost(size_t current_level)
    {
        return get_stat_point_improvement_cost(current_level, current_level + 1);
    }

    constexpr int max_disadvantage_point_minus = -40;  //or is it -30? todo: look up in Gurps Basic
    constexpr size_t max_quirks = 5;

    enum encumberance_e
    {
          encumberance_none = 0
        , encumberance_light
        , encumberance_medium
        , encumberance_heavy
        , encumberance_extra_heavy
        , encumberance_count
    };

    constexpr std::array<int, encumberance_extra_heavy + 1> encumberance_mults = 
    {
           2 //encumberance_none
        ,  4 //encumberance_light
        ,  8 //encumberance_medium
        , 12 //encumberance_heavy
        , 20 //encumberance_extra_heavy
    };

    constexpr std::array<const char*, encumberance_count> encumberance_names =
    {
          "None"
        , "Light"
        , "Medium"
        , "Heavy"
        , "Extra Heavy"
    };

    constexpr float get_encumberance_weight(const int ST, const encumberance_e encumberance)
    {
        return ST * encumberance_mults[encumberance];
    }

    MSVC_CONSTEXPR encumberance_e get_encumberance(const int ST, const float weight)
    {
        for(size_t i = 0; i < encumberance_mults.size(); ++i)
        {
            if(weight <= encumberance_mults[i] * ST)
            {
                return encumberance_e(i);
            }
        }

        return encumberance_extra_heavy;
    }


    /*************************************************/
    /*** DAMAGE TYPE INFO
    /*************************************************/
    enum damage_type_e
    {
          damage_pure = 0    // armor peircing
        , damage_crushing
        , damage_cutting
        , damage_impaling
        , damage_lightning   // ignores most DR from metal armor
        , damage_type_count
    };

    constexpr std::array<const char*, damage_type_count> damage_type_names =
    {
          "pure"
        , "crushing"
        , "cutting"
        , "impaling"
        , "lightning"
    };

    constexpr std::array<const char*, damage_type_count> damage_type_names_short =
    {
          "pure"
        , "cr"
        , "cut"
        , "imp"
        , "zap"
    };

    constexpr std::array<float, damage_type_count> damage_type_mults =
    {
          1.0f // damage_pure
        , 1.0f // damage_crushing
        , 1.5f // damage_cutting
        , 2.0f // damage_impaling
        , 1.0f // damage_lightning
    };


    

    /*************************************************/
    /*** HIT LOCATION INFO
    /*************************************************/
    enum hit_location_e
    {
          hit_locaiton_torso = 0
        , hit_location_torso_vital_organ
        , hit_location_head
        , hit_location_brain
        , hit_location_eyes
        , hit_location_arm_left
        , hit_location_arm_right
        , hit_location_hand_left
        , hit_location_hand_right
        , hit_location_weapon
        , hit_location_leg_left
        , hit_location_leg_right
        , hit_location_foot_left
        , hit_location_foot_right
        , hit_location_count
    };

    constexpr std::array<int, hit_location_count> hit_location_penalties =
    {
           0 // hit_locaiton_torso
        , -3 // hit_location_torso_vital_organ
        , -5 // hit_location_head
        , -7 // hit_location_brain
        , -9 // hit_location_eyes
        , -2 // hit_location_arm_left
        , -2 // hit_location_arm_right
        , -4 // hit_location_hand_left
        , -4 // hit_location_hand_right
        , -2 // hit_location_leg_left
        , -2 // hit_location_leg_right
        , -4 // hit_location_foot_left
        , -4 // hit_location_foot_right
    };

    /*************************************************/
    /*** ATTACK INFO
    /*************************************************/
    enum attack_position_e
    {
          attack_standing = 0
        , attack_crouching
        , attack_kneeling
        , attack_sitting
        , attack_crawling
        , attack_laying_down
        , attack_position_count
    };

    constexpr std::array<int, attack_position_count> attack_position_penalty =
    {
           0 //attack_standing
        , -2 //attack_crouching
        , -2 //attack_kneeling
        , -2 //attack_sitting
        , -4 //attack_crawling
        , -4 //attack_laying_down
    };


    /*************************************************/
    /*** ARMOR INFO
    /*************************************************/
    enum armor_type_e
    {
          armor_none = 0
        , armor_clothing
        , armor_winter_clothing
        , armor_padded_cloth
        , armor_light_leather
        , armor_heavy_leather
        , armor_chainmail
        , armor_scale
        , armor_light_plate
        , armor_heavy_plate
        // , armor_flak_jacket
        // , armor_light_kevlar
        // , armor_heavy_kevlar
        // , todo: the rest of the armor post TL4
        , armor_info_count
    };

    struct armor_info_t
    {
        size_t TL_min;// = 0; //tech level min
        size_t TL_max;// = 0; //tech level max
        size_t PD;// = 0; //passive defense
        size_t DR;// = 0; //damage resistance
    };

    constexpr std::array<armor_info_t, armor_info_count> armor_info = 
    {
          armor_info_t{0,0,  0,0} // armor_none
        , armor_info_t{0,0,  0,0} // armor_clothing
        , armor_info_t{0,0,  1,1} // armor_winter_clothing
        , armor_info_t{1,4,  1,1} // armor_padded_cloth
        , armor_info_t{1,4,  2,1} // armor_light_leather
        , armor_info_t{1,4,  3,2} // armor_heavy_leather
        , armor_info_t{3,4,  3,4} // armor_chainmail
        , armor_info_t{2,4,  4,4} // armor_scale
        , armor_info_t{3,4,  4,5} // armor_light_plate
        , armor_info_t{3,4,  4,7} // armor_heavy_plate
    };

    enum armor_region_e
    {
          armor_region_none = 0  //used for hit_location_weapon
        , armor_region_head
        , armor_region_body
        , armor_region_arms
        , armor_region_legs
        , armor_region_hands
        , armor_region_feet
        , armor_region_count
    };

    constexpr std::array<const char*, armor_region_count> armor_region_names = 
    {
          "" //armor_region_none
        , "Head"
        , "Body"
        , "Arms"
        , "Legs"
        , "Hands"
        , "Feet"
    };

    constexpr std::array<const char*, armor_region_count> armor_region_initials = 
    {
          "" //armor_region_none
        , "H"
        , "B"
        , "A"
        , "L"
        , "H"
        , "F"
    };

	MSVC_CONSTEXPR armor_region_e armor_region_from_hit_location(const hit_location_e hit_location)
    {
        switch (hit_location)
        {
        case hit_locaiton_torso: [[clang::fallthrough]];
        case hit_location_torso_vital_organ:
            return armor_region_body;

        case hit_location_head: [[clang::fallthrough]];
        case hit_location_brain: [[clang::fallthrough]];
        case hit_location_eyes:
            return armor_region_head;

        case hit_location_arm_left: [[clang::fallthrough]];
        case hit_location_arm_right:
            return armor_region_arms;

        case hit_location_hand_left: [[clang::fallthrough]];
        case hit_location_hand_right:
            return armor_region_hands;

        case hit_location_leg_left: [[clang::fallthrough]];
        case hit_location_leg_right:
            return armor_region_legs;

        case hit_location_foot_left: [[clang::fallthrough]];
        case hit_location_foot_right:
            return armor_region_feet;

        case hit_location_weapon:
            return armor_region_none;

        default:
            return armor_region_none;
        }
    }

    MSVC_CONSTEXPR int calc_damage(size_t damage, damage_type_e damage_type, armor_type_e armor_type, int DR_mod)
    {
        switch(damage_type)
        {
            case damage_pure: //ignore armor
            {
                return damage;
            }

            case damage_lightning:
            {
                //all but 1 damage ignored for metal armor  (might be all armor ignored?)
                if(armor_type >= armor_chainmail && armor_type <= armor_heavy_plate)
                {
                    return (damage - glm::min(damage, armor_info[armor_type].DR));
                }
            }
            [[clang::fallthrough]];

            case damage_crushing: [[clang::fallthrough]];
            case damage_cutting:  [[clang::fallthrough]];
            case damage_impaling:
            {
                return (damage - glm::min(damage, armor_info[armor_type].DR)) * damage_type_mults[damage_type];
            }

            default:
                return damage;
        }
    }



    enum defense_type_e
    {
          defense_dodge
        , defense_parry
        , defense_block
    };


    /*************************************************/
    /*** CURRENCY INFO
    /*************************************************/
    enum currency_type_e
    {
          copper = 0
        , silver
        , gold
        , platinum
        , currency_type_count
    };

    // std::array<float, currency_type_count> currency_value =
    // {
    //          1.0f //copper
    //     ,   20.0f //silver
    //     ,  200.0f //gold
    //     , 1000.0f //platinum  ??? Unknown Value so far, guessing 1000
    // };

    // float operator "" _copper  (float amt) { return amt * currency_value[copper]; }
    // float operator "" _silver  (float amt) { return amt * currency_value[silver]; }
    // float operator "" _gold    (float amt) { return amt * currency_value[gold]; }
    // float operator "" _platinum(float amt) { return amt * currency_value[platinum]; }

    // /*constexpr*/ float convert_currency(const float amt, currency_type_e from_type, currency_type_e to_type = copper)
    // {
    //     //convert from_amt to copper and then from copper to to_amt
    //     return (amt * currency_value[from_type]) / currency_value[to_type];
    // }
}
}

#pragma clang diagnostic pop
