#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "asdf_multiplat/data/texture_atlas.h"

#include "data/hex_grid.h"
#include "data/map_objects.h"
#include "data/spline.h"
#include "data/terrain_bank.h"

namespace asdf
{
namespace hexmap
{
namespace data
{
    using object_index_t = size_t;

    struct hex_map_t
    {
        std::string map_name = "";

        data::hex_grid_t hex_grid;
        std::vector<map_object_t> objects;
        std::vector<spline_t> splines;

        data::terrain_bank_t terrain_bank;
        std::unique_ptr<asdf::data::texture_atlas_t> objects_atlas;
        
        hex_map_t(std::string const& map_name, glm::uvec2 grid_size, hex_grid_cell_t const& default_cell_style = hex_grid_cell_t{});
        hex_map_t(glm::uvec2 grid_size);

        object_index_t object_index_at(glm::vec2 const& world_pos) const;
        spline_index_t spline_index_at(glm::vec2 const& world_pos) const;
        std::vector<object_index_t> object_indices_at(glm::vec2 const& world_pos) const;
        std::vector<spline_index_t> spline_indices_at(glm::vec2 const& world_pos) const;

        void save_to_file(std::string const& filepath);
        void load_from_file(std::string const& filepath);

    private:
        void _save_to_file(std::experimental::filesystem::path const& filepath);
        void _load_from_file(std::experimental::filesystem::path const& filepath);
        void package_map(std::vector<std::experimental::filesystem::path> const& map_filepaths
                       , std::experimental::filesystem::path const& package_filepath);
        void unpackage_map(std::experimental::filesystem::path const& filepath);
    };

    constexpr size_t hxm_version = 0;

    struct hxm_header_v0
    {
        uint64_t version{0}; //this might need to be seperate from the header in case the header changes

        glm::uvec2 chunk_size{0};
        glm::uvec2 map_size{0};
        uint64_t num_map_objects{0};
        uint64_t num_splines{0};
    };

    struct hxm_header_t : hxm_header_v0
    {};
}
}
}