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

        size_t object_index_at(glm::vec2 world_pos);
    };
}
}
}