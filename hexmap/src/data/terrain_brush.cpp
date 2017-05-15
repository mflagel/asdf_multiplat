#include "terrain_brush.h"

#include "asdf_multiplat/main/asdf_defs.h"

#include "data/hex_util.hpp"
#include "ui/hex_grid.h"

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

    size_t terrain_brush_t::num_empty_hexes() const
    {
        size_t num_empty = 0;

        for(int y = 0; y < size().y; ++y)
        {
            for(int x = 0; x < size().x; ++x)
            {
                num_empty += cell_is_empty(x,y);
            }
        }

        return num_empty;
    }
}

namespace ui
{
    polygon_<terrain_brush_vertex_t> verts_for_terrain_brush(data::terrain_brush_t const& brush)
    {
        polygon_<terrain_brush_vertex_t> brush_verts;
        brush_verts.resize(brush.num_hexes() * 6);

        size_t hex_count = 0;

        for(int y = 0; y < brush.size().y; ++y)
        {
            for(int x = 0; x < brush.size().x; ++x)
            {
                if(!brush.cell_is_empty(x,y))
                {
                    auto* verts = brush_verts.data() + (hex_count * 6);

                    verts[hex_count].position.x = hexagon_points[hex_count*6 + 0];
                    verts[hex_count].position.y = hexagon_points[hex_count*6 + 1];
                    verts[hex_count].position.z = hexagon_points[hex_count*6 + 2];

                    ++hex_count;
                }
            }
        }

        return brush_verts;
    }
}
}
}