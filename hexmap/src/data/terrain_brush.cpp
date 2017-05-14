#include "terrain_brush.h"

#include "asdf_multiplat/main/asdf_defs.h"

#include "data/hex_util.hpp"

using namespace std;
using namespace glm;

namespace asdf
{
namespace hexmap
{
namespace data
{
    terrain_brush_t::terrain_brush_t()
    : brush_mask(glm::uvec2{1,1}, hex_grid_cell_t{1})
    {
    }

    terrain_brush_t::terrain_brush_t(uvec2 size, bool init_val)
    : brush_mask(size, hex_grid_cell_t{init_val ? 1u : 0u})
    {
        WARN_IF(size.x > 0 && size.y > 0, "terrain brush size is zero");
    }


    terrain_brush_t terrain_brush_circle(float radius)
    {
        terrain_brush_t brush(glm::uvec2{radius * 2, radius * 2});

        for(size_t y = 0; y < radius * 2; ++y)
        {
            for(size_t x = 0; x < radius * 2; ++x)
            {
                auto v = glm::vec2(x,y) - glm::vec2(radius,radius);
                brush.brush_mask.cell_at_local_coord(x,y).tile_id = length(v) <= radius;
            }
        }

        return brush;
    }

    terrain_brush_t terrain_brush_hexagon(float radius)
    {
        terrain_brush_t brush(glm::uvec2{radius * 2, radius * 2});

        auto spiral = cube_spiral(cube_coord_t{0}, radius);

        for(auto const& cube_coord : spiral)
        {
            auto hex_coord = cube_to_hex(cube_coord);

            //shift 0,0 to center of brush
            //ie: make all coord values positive
            hex_coord.x += radius;
            hex_coord.y += radius;

            ASSERT(hex_coord.x > 0 && hex_coord.y > 0, "cannot use negative coords when making a brush");

            auto& cell = brush.brush_mask.cell_at_local_coord(hex_coord);
            cell.tile_id = 1;
        }

        return brush;
    }

    //terrain_brush_t terrain_brush_from_bitmap(std::string const& filepath)
    //{
    //    EXPLODE("TODO");
    //    return terrain_brush_t();
    //}

}
}
}