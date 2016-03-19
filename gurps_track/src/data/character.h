#pragma once

#include "stdafx.h"
#include "basic_data.h"
#include "skills.h"

struct cJSON;

namespace gurps_track
{
namespace data
{
    enum effect_type_e
    {
          effect_base_stat = 0
        , effect_derived_stat
        , effect_special_stat
        , effect_type_count
    };

    constexpr std::array<size_t, effect_type_count> effect_type_indexes =
    {
          0                 // base stats start at 0
        , base_stat_count   // derived stats start after base stats
        , base_stat_count + derived_stat_count
    };

    struct status_effect_t
    {
        //in theory I could make this a bool but I figure another type might exist later

        std::string name;
        effect_type_e effect_type; //in theory I could use a variant instead
        int type_index; //ie: stat_ST, stat_hearing, etc
        int amount;

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

    /// An   Advantage is any trait_t with a positive point value
    /// A Disadvantage is any trait_t with a negative point value lower than -1
    /// A Quirk        is any trait_t with a          point value of 0 or -1
    struct trait_t
    {
        std::string name;
        std::string description;
        int point_cost;

        std::vector<status_effect_t> status_effects;

        bool is_advantage() const;
        bool is_disadvantage() const;
        bool is_quirk() const;

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

    struct race_t
    {
        std::string name;
        std::string description;

        std::vector<trait_t> traits;
        int point_cost() const;

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

    //todo: move this somewhere more generic
    //todo: make a to_string overload for this (and support user_defined to_string overloads in general)
    struct date_t
    {
        uint8 day = 0;
        uint8 month = 0;
        uint16 year = 0;

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);

        inline std::string to_string() const
        {
            return std::to_string(day) + " / " + std::to_string(month) + " / " + std::to_string(year);
        }
    };

    struct char_point_transaction_t
    {
        int amount = 0;
        std::string description = "";
        date_t date;

        char_point_transaction_t() = default;
        char_point_transaction_t(int _amount, std::string _description = "", date_t _date = date_t());

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

    struct armor_t
    {
        armor_type_e type = armor_none;
        int PD_mod = 0;
        int DR_mod = 0;
    };

    struct character_info_t
    {
        std::string name = "";
        std::string description = "";

        std::array<int, base_stat_count> base_stats;
        std::array<int, derived_stat_count> derived_stats;

        int current_fatigue =  0;
        int current_HP = 0;

        race_t race;
        std::vector<trait_t> traits; //advantages, disadvantages, and quirks

        learned_skills_list_t skills;
        learned_skills_list_t spells;

        std::vector<status_effect_t> status_effects; //currently active spells/ailments/etc

        character_info_t();
    };

    struct character_t : character_info_t
    {
        static constexpr size_t save_format_version = 0;

        size_t version = 0;
        date_t date_modified;

        std::string character_filepath = "";
        std::string portrait_filepath = "";

        std::vector<char_point_transaction_t> point_transactions;
        int unspent_points = -1; //cached sum of point_transactions;

        //this will eventually be replaced with gear section
        float weight_combat = 0.0f;
        float weight_pack = 0.0f;

        std::array<armor_t, armor_region_count> armor_info; //this will eventually be derived from gear and status effects
        std::array<int, base_stat_count + derived_stat_count> stat_mod_cache;

        character_t(std::string filepath);

        int ST() const { return get_stat(stat_ST); }
        int DX() const { return get_stat(stat_DX); }
        int IQ() const { return get_stat(stat_IQ); }
        int HT() const { return get_stat(stat_HT); }
        // int magery() const      { return get_stat(stat_magery);  }
        int max_HP() const      { return get_stat(stat_HP);      }
        int max_fatigue() const { return get_stat(stat_fatigue); }
        float basic_speed() const { return static_cast<float>(DX() + HT()) / 4.0f; }
        int movement() const      { return static_cast<int>(basic_speed()); } //truncate
        
        void derive_stats();
        void cache_mods();

        void add_effect(const status_effect_t);
        // bool remove_effect(const status_effect_t);
        // bool remove_effect(status_effect_t*);
        // bool remove_effect(size_t index);

        int get_stat(base_stat_e) const;
        int get_stat(derived_stat_e) const;
        int get_cached_mod(base_stat_e) const;
        int get_cached_mod(derived_stat_e) const;

        // encumberance_e get_encumberance() const;

        int PD(armor_region_e) const;
        int DR(armor_region_e) const;

        int stats_point_cost() const;
        int advantages_point_cost() const;
        int disadvantages_point_cost() const;
        int quirks_point_cost() const;
        int traits_point_cost() const;
        double skills_point_cost() const;
        int spells_point_cost() const;
        double total_point_cost() const;

        void spend_points(uint16 amount, std::string memo);
        void gain_points(uint16 amount, std::string memo);
        void points_transaction(int amount, std::string memo);

        void heal_damage(int amount_healed);
        void take_damage(int damage_taken, damage_type_e, hit_location_e);

        inline bool has_skill(skill_t const& skill) const { return has_skill(skill.name); }
        bool has_skill(std::string const& skill_name) const;
        void learn_skill(skill_t, int initial_points = 0);
        
        bool can_improve_skill(size_t listing_index/*, int amount = 1*/) const;
        bool improve_skill(size_t listing_index/*, int amount = 1*/);

        void load_default_data();
        void load_test_data();
        void load_from_file(std::string filepath);
        void save_to_file(std::string filepath);
        void save_data();
        
        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };
}
}