#pragma once

#include <string>
#include <map>
#include <vector>

namespace fast_travel_sim
{
    enum terrain_grade_e
    {
        terrain_flat = 0
      , terrain_hilly
      , terrain_mountainous
      , terrain_marshland
      , terrain_grade_count
    };

    inline constexpr int32_t travel_rate(terrain_grade_e tg)
    {
        return tg - terrain_grade_count
    }

    enum path_type_e
    {
        path_paved = 0
      , path_dirt
      , path_forest
      , path_difficult
    };

    using hex_coord_t = glm::ivec2;

    struct hex_t
    {
        hex_coord_t coord;

        terrain_grade_e grade = terrain_flat;
        path_type_e     path  = path_dirt;
    };


    // struct location_encounter_t
    // {
    // };

    // struct plant_encounter_t
    // {
    // };

    // struct combat_encounter_t
    // {
    // };


    using location_encounter_t = std::string;
    using    plant_encounter_t = std::string;
    using creature_encounter_t = std::string;

    struct generated_hex_t : hex_t
    {
        location_encounter_t location;
        plant_encounter_t    plant;
        creature_encounter_t creature;

        generated_hex_t(hex_t const&, pregen_node_t const& pregen_node);
    }

    struct hex_database_t
    {
        std::map<hex_coord_t, generated_hex_t> hex_data;
        pregen_node_t hex_rollables;

        void generate_if_empty(hex_t const& coord);
        void generate_if_empty(std::vector<hex_t> const& hexes);
    };

    bool is_boring(generated_hex_t const&);
    bool is_boring(journal_entry_t const&);
}