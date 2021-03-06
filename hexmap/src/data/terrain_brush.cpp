#include "terrain_brush.h"

#include <asdfm/main/asdf_defs.h>

#include "data/hex_util.h"
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
        WARN_IF(size.x == 0 && size.y == 0, "terrain brush size is zero");
    }

    bool terrain_brush_t::operator==(terrain_brush_t const& rhs)
    {
        if(size() != rhs.size())
            return false;

        return brush_mask != rhs.brush_mask;
    }


    /// Brush Creation Functions
    terrain_brush_t terrain_brush_rectangle(int w, int h)
    {
        return terrain_brush_t(glm::uvec2(w, h), true);
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

    terrain_brush_t terrain_brush_hexagon(int radius)
    {
        terrain_brush_t brush(glm::uvec2{radius * 2 + 1, radius * 2 + 1});

        //auto spiral = cube_spiral(cube_coord_t{0}, radius);
        auto range = cube_range(cube_coord_t(0), radius);

        for(auto const& cube_coord : range)
        {
            auto hex_coord = cube_to_hex(cube_coord);

            //shift 0,0 to center of brush
            //ie: make all coord values positive
            hex_coord.x += radius;
            hex_coord.y += radius;

            ASSERT(hex_coord.x >= 0 && hex_coord.y >= 0, "cannot use negative coords when making a brush");

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

    std::vector<glm::ivec2> get_brush_grid_overlap(terrain_brush_t const& brush, hex_grid_t const& grid, hex_coord_t overlap_center)
    {
        auto brush_halfsize = glm::ivec2(brush.size()) / 2;
        auto brush_coord_to_grid_coord = [&grid, &overlap_center, brush_halfsize](glm::ivec2 const& brush_coord) -> glm::ivec2
        {
            auto grid_coord = brush_coord - brush_halfsize + overlap_center;
            grid_coord.y += ((overlap_center.x+1) & 1) * (grid_coord.x & 1); //shift up if overla_center is even and grid_coord is odd
            return grid_coord;
        };


        std::vector<glm::ivec2> overlap;
        overlap.reserve(brush.num_non_empty_hexes());

        for(int brush_y = 0; brush_y < brush.size().y; ++brush_y)
        {
            for(int brush_x = 0; brush_x < brush.size().x; ++brush_x)
            {
                if(!brush.cell_is_empty(brush_x, brush_y))
                {
                    auto adjusted_coord = brush_coord_to_grid_coord(glm::ivec2(brush_x, brush_y));

                    if(grid.is_in_bounds(adjusted_coord))
                    {
                        overlap.emplace_back(adjusted_coord.x, adjusted_coord.y);
                    }
                }
            }
        }

        return overlap;
    }

}
}
}