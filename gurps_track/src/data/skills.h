#pragma once

CLANG_DIAGNOSTIC_PUSH
CLANG_DIAGNOSTIC_IGNORE("-Wc++11-compat-deprecated-writable-strings")

#include "basic_data.h"

struct cJSON;

namespace gurps_track
{
namespace data
{
    // will replace this with data driven categories so it can be more customizable
    // keeping the code here for easy copypasta later
    /*enum skill_category_e
    {
          skill_category_none          = 0
        , skill_category_animal        = 0x1
        , skill_category_artistic      = 0x2
        , skill_category_athletic      = 0x4
        , skill_category_combat_weapon = 0x8
        , skill_category_craft         = 0x10
        , skill_category_hobby         = 0x20
        , skill_category_language      = 0x40
        , skill_category_magical       = 0x80
        , skill_category_medical       = 0x100
        , skill_category_outdoor       = 0x200
        , skill_category_professional  = 0x400
        , skill_category_psionic       = 0x800
        , skill_category_scientific    = 0x1000
        , skill_category_social        = 0x2000
        , skill_category_theif_spy     = 0x4000
        , skill_category_vehicle       = 0x8000
        , skill_category_count
    };

    constexpr std::array<const char*, skill_category_count> skill_category_names =
    {
          "None"
        , "Animal"
        , "Artistic"
        , "Athletic"
        , "Combat/Weapon"
        , "Craft"
        , "Hobby"
        , "Language"
        , "Magical"
        , "Medical"
        , "Outdoor"
        , "Professional"
        , "Psionic"
        , "Scientific"
        , "Social"
        , "Theif/Spy"
        , "Vehicle"
    };
    */

    enum skill_difficulty_e
    {
          skill_difficulty_easy
        , skill_difficulty_average
        , skill_difficulty_hard
        , skill_difficulty_very_hard
        , skill_difficulty_count
    };

    constexpr std::array<const char*, skill_difficulty_count> skill_difficulty_abbreviations =
    {
          "E"
        , "A"
        , "H"
        , "VH"
    };

    constexpr std::array<int, skill_difficulty_count> skill_start_value =
    {
          -1 // E  easy skills start at -1 at half a point
        , -2 // A  average start at -2
        , -3 // H  etc...
        , -4 // VH
    };

    constexpr std::array<int, 6> physical_skill_cost_curve =
    {
          0
        , 1
        , 2
        , 4
        , 8
        , 16
    };

    constexpr std::array<int, 6> mental_skill_cost_curve =
    {
          0
        , 1
        , 2
        , 4
        , 6
        , 8
    };

    constexpr std::array<int, 6> mental_VH_skill_cost_curve =
    {
          0
        , 1
        , 2
        , 4
        , 8
        , 12
    };

    MSVC_CONSTEXPR std::array<int, 6> get_skill_cost_curve(skill_difficulty_e difficulty, base_stat_e stat)
	{
	    if(stat == stat_IQ)
	    {
	    	if(difficulty == skill_difficulty_very_hard)
	    	{
	    	    return mental_VH_skill_cost_curve;
	    	}

	    	return mental_skill_cost_curve;
	    }
	    else
	    {
	        return physical_skill_cost_curve;
	    }
	}

    size_t get_skill_cost_index(skill_difficulty_e, base_stat_e, int point_cost);
    int point_cost_from_index(skill_difficulty_e, base_stat_e, size_t cost_index);

    int get_skill_improvement_cost(skill_difficulty_e, base_stat_e, int point_cost_start, int point_cost_end);


    struct skill_default_t
    {
        std::string name;
        int penalty;

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

    struct skill_t
    {
        std::string name;
        std::string description;

        base_stat_e base_stat;
        skill_difficulty_e difficulty;
        //int min_tech_level

        std::string defaults;
        // std::vector<skill_default_t> defaults;
        //std::vector<std::string> prerequisites

        bool is_physical() const;
        bool is_mental() const;
        std::string difficulty_string() const;

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

    struct spell_t
    {
        enum duration_type_e
        {
              duration_normal = 0
            , duration_permenant
            , duration_block
        };

        std::string name;
        std::string description;
        skill_difficulty_e difficulty;

        // using strings, since sometimes the values are more complicated
        // ex: variable costs or times to cast found in Teleport or Dispel Magic
        // Will use some sort of special character to signify values that are
        // modified by higher spell level.
        std::string initial_cost;
        std::string maintenance_cost;
        std::string duration;
        std::string time_to_cast;

        duration_type_e duration_type;
        defense_type_e defense_type;   //only applicable if duration_block type spell

        std::string info;

        std::string difficulty_string() const;

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

    
    struct skill_bonus_t
    {
        int bonus_amount;
    };

    struct character_t;


    struct learned_skill_t
    {
        skill_t skill;
        size_t num_improvements = 0;

        learned_skill_t() = default;  //default ctor required for serialization

        // default for initial will be 0 for GURPS 3rd,  1 for 4th
        learned_skill_t(skill_t _skill, int _initial_improvement_level)
        : skill(std::move(_skill))
        , num_improvements(_initial_improvement_level)
        {
        }

        inline int point_cost() const       { return point_cost_from_index(skill.difficulty, skill.base_stat, num_improvements); }
        inline int improvement_cost() const { return point_cost_from_index(skill.difficulty, skill.base_stat, num_improvements + 1) - point_cost(); }

        inline int get_effective_skill(int stat_value, int modifiers = 0) const { return stat_value + skill_start_value[skill.difficulty] + num_improvements + modifiers; }
        int get_effective_skill(character_t const& character) const;

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

    // not really worth templating vs copy-paste + edit, since there will only ever 
    // be two types (learned spell and learned skill
    struct learned_spell_t
    {
        spell_t spell;
        size_t num_improvements = 1;

        learned_spell_t(spell_t _spell, int _initial_improvement_level = 1)
        : spell(std::move(_spell))
        , num_improvements(_initial_improvement_level)
        {
        }

        inline int point_cost() const       { return point_cost_from_index(spell.difficulty, stat_IQ, num_improvements); }
        inline int improvement_cost() const { return point_cost_from_index(spell.difficulty, stat_IQ, num_improvements + 1) - point_cost(); }

        inline int get_effective_skill(int stat_value, int modifiers = 0) const { return stat_IQ + skill_start_value[spell.difficulty] + num_improvements + modifiers; }
        int get_effective_skill(character_t const& character) const;

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

    using learned_skills_list_t = std::vector<learned_skill_t>;
    using learned_spells_list_t = std::vector<learned_spell_t>;

    struct skill_list_t : std::vector<skill_t>
    {
        using std::vector<skill_t>::vector;

        skill_list_t(){}
        skill_list_t(std::string const& filepath);

        void from_JSON(cJSON*); // defined in to_from_json.cpp
    };

    struct spell_list_t : std::vector<spell_t>
    {
        using std::vector<spell_t>::vector;

        spell_list_t(){}
        spell_list_t(std::string const& filepath);

        void from_JSON(cJSON*); // defined in to_from_json.cpp
    };
}
}

CLANG_DIAGNOSTIC_POP
