#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "data/hex_grid.h"

namespace asdf
{
namespace hexmap
{
namespace data
{
    struct terrain_brush_t
    {
        hex_grid_chunk_t brush_mask;

        terrain_brush_t();
        terrain_brush_t(glm::uvec2 size, bool start_val = false);
    };

    terrain_brush_t terrain_brush_circle(float radius);
    terrain_brush_t terrain_brush_hexagon(float radius);
    //terrain_brush_t terrain_brush_from_bitmap(std::string const& filepath);
}
}
}