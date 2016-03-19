#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"

#include "basic_data.h"

struct cJSON;

namespace gurps_track
{
namespace data
{
    enum skill_category_e
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

    enum skill_difficulty_e
    {
          physical_easy = 0
        , physical_average
        , physical_hard
        , mental_easy
        , mental_average
        , mental_hard
        , mental_very_hard
        , skill_difficulty_count
    };

    constexpr std::array<const char*, skill_difficulty_count> skill_difficulty_abbreviations =
    {
          "P/E"
        , "P/A"
        , "P/H"
        , "M/E"
        , "M/A"
        , "M/H"
        , "M/VH"
    };

    constexpr bool is_physical(skill_difficulty_e difficulty)
    {
        return int(difficulty) >= physical_easy && int(difficulty) <= physical_hard;
    }

    constexpr bool is_mental(skill_difficulty_e difficulty)
    {
        return int(difficulty) >= mental_easy && int(difficulty) <= mental_hard;
    }

    constexpr std::array<int, skill_difficulty_count> skill_start_value =
    {
          -1 // P/E  easy skills start at -1 at half a point
        , -2 // P/A  average start at -2
        , -3 // P/H  etc...
        , -1 // M/E
        , -2 // M/A
        , -3 // M/H
        , -4 // M/VH
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

    constexpr std::array<int, 6> get_skill_cost_curve(skill_difficulty_e difficulty);

    size_t get_skill_cost_index(skill_difficulty_e difficulty, int point_cost);
    int point_cost_from_index(skill_difficulty_e difficulty, size_t cost_index);

    int get_skill_improvement_cost(skill_difficulty_e difficulty, int point_cost_start, int point_cost_end);


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
        skill_difficulty_e difficulty;
        //int min_tech_level

        std::string defaults;
        // std::vector<skill_default_t> defaults;
        //std::vector<std::string> prerequisites

        uint32 categories;

        bool is_physical() const;
        bool is_mental() const;

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

    struct spell_t : skill_t
    {
        enum duration_type_e
        {
              duration_normal = 0
            , duration_permenant
            , duration_block
        };

        size_t initial_cost;
        size_t maintenance_cost;
        size_t duration;
        size_t time_to_cast;

        duration_type_e duration_type;
        defense_type_e defense_type;   //only applicable if duration_block type spell

        std::string info;
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

        learned_skill_t(skill_t _skill, int _initial_improvement_level);

        int point_cost() const;
        int improvement_cost() const;

        int get_effective_skill(int stat_value, int modifiers = 0) const;
        int get_effective_skill(character_t* character) const;
    };

    // todo: use something like a map so I can do something more efficient than increment search
    // potentially an unordered_map<string, skill_listing>
    // struct learned_skills_list_t
    // {
    //     std::vector<skill_listing_t> skills;

    //     learned_skills_list_t get_skill(std::string name);
    // };
    using learned_skills_list_t = std::vector<learned_skill_t>;

    struct skill_list_t : std::vector<skill_t>
    {
        using std::vector<skill_t>::vector;

        skill_list_t(){}
        skill_list_t(std::string const& filepath);

        void from_JSON(cJSON*); // defined in to_from_json.cpp
    };
}
}

#pragma clang diagnostic pop
