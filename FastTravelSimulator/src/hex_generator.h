#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <asdfm/utilities/utilities.h>
#include <plantgen/plant_generator.h>

namespace fast_travel_sim
{
    using pregen_node_t = plantgen::pregen_node_t;
    using generated_node_t = plantgen::generated_node_t;

    struct journal_entry_t;

    /////////////////////////////////

    enum terrain_grade_e
    {
        terrain_flat = 0
      , terrain_hilly
      , terrain_mountainous
      , terrain_marshland
      , terrain_grade_count
    };

    enum path_type_e
    {
        path_paved = 0
      , path_dirt
      , path_none
      , path_forest
      , path_difficult
    };

    /// I could make path_type_e start at -2 and count upwards
    /// but I prefer starting at 0 in case I want to index into
    /// an array (it's common for me to have a constexpr array
    /// of names)
    constexpr int32_t path_difficulty_min = -2;
    inline constexpr int32_t path_difficulty(path_type_e path)
    {
        return path_difficulty_min + path;
    }

    using hex_coord_t = glm::ivec2;

    struct hex_t
    {
        hex_coord_t coord;
        terrain_grade_e grade = terrain_flat;

        hex_t() = default;
        hex_t(hex_t const&) = default;
    };

    inline int32_t terrain_difficulty(terrain_grade_e grade, path_type_e path)
    {
        return uint32_t(grade) + path_difficulty(path);
    }

    inline int32_t terrain_difficulty(hex_t const& hex)
    {
        //return terrain_difficulty(hex.grade, hex.path);
        return int32_t(hex.grade);
    }

    /// Define these as structs instead of just type aliases
    /// so that I can overload a summarize function for each of these
    struct location_encounter_t : generated_node_t
    {
        location_encounter_t() = default;
        location_encounter_t(generated_node_t const& g)
        : generated_node_t(g)
        {}
    };

    struct plant_encounter_t : generated_node_t
    {
        plant_encounter_t() = default;
        plant_encounter_t(generated_node_t const& g)
        : generated_node_t(g)
        {
        }
    };
    struct creature_encounter_t : generated_node_t
    {
        creature_encounter_t() = default;
        creature_encounter_t(generated_node_t const& g)
        : generated_node_t(g)
        {}
    };

    struct generated_hex_t : hex_t
    {
        location_encounter_t location;
        plant_encounter_t    plant;

        generated_hex_t(hex_t const&, pregen_node_t const& pregen_node);
    };

    struct hex_database_t
    {
        /// hash function for hex_coord_t (ie: glm::ivec2) is in hexmap/data/hex_utils.h
        std::unordered_map<hex_coord_t, generated_hex_t> hex_data;
        pregen_node_t hex_rollables;
        pregen_node_t creature_rollables;

        inline generated_hex_t& hex_at(hex_coord_t c)
        {
            ASSERT(hex_data.count(c) == 1, "accessing a hex that has not been generated yet");
            return hex_data.at(c);
        }
        inline generated_hex_t const& hex_at(hex_coord_t c) const
        {
            ASSERT(hex_data.count(c) == 1, "accessing a hex that has not been generated yet");
            return hex_data.at(c);
        }

        void generate_if_empty(hex_t const& coord);
        void generate_if_empty(std::vector<hex_t> const& hexes);
    };

    // bool is_boring(generated_hex_t const&);
    // bool is_boring(journal_entry_t const&);
}