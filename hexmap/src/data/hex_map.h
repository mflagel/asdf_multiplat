#pragma once

#include <glm/glm.hpp>

#include "data/hex_grid.h"
#include "data/map_objects.h"
#include "data/spline.h"

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
        
        hex_map_t(std::string const& map_name, glm::uvec2 grid_size, hex_grid_cell_t const& default_cell_style = hex_grid_cell_t{});
        hex_map_t(glm::uvec2 grid_size);
        void save_to_file(std::string const& filepath);
        void load_from_file(std::string const& filepath);

        object_index_t object_index_at(glm::vec2 const& world_pos) const;
        spline_index_t spline_index_at(glm::vec2 const& world_pos) const;
        std::vector<object_index_t> object_indices_at(glm::vec2 const& world_pos) const;
        std::vector<spline_index_t> spline_indices_at(glm::vec2 const& world_pos) const;
    };

    struct hxm_header_t
    {
        uint64_t version{0}; //this might need to be seperate from the header in case the header changes

        glm::uvec2 map_size{0};
        glm::uvec2 chunk_size{0};
        uint64_t num_map_objects{0};
        uint64_t num_splines{0};
    };
}
}
}