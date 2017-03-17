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
    struct hex_map_t
    {
        data::hex_grid_t hex_grid;
        std::vector<map_object_t> objects;
        std::vector<spline_t> splines;
        
        hex_map_t(glm::uvec2 grid_size);
        void save_to_file(std::string const& filepath);
        void load_from_file(std::string const& filepath);

        size_t object_index_at(glm::vec2 const& world_pos) const;
        std::vector<size_t> object_indices_at(glm::vec2 const& world_pos) const;
    };

    struct hxm_header_t
    {
        size_t version{0}; //this might need to be seperate from the header in case the header changes

        glm::uvec2 map_size{0};
        glm::uvec2 chunk_size{0};
        size_t num_map_objects{0};
        size_t num_splines{0};
    };
}
}
}