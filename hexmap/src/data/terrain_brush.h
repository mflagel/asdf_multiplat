#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "asdf_multiplat/ui/polygon.h"

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

        inline glm::uvec2 size() const { return brush_mask.size; }

        inline bool cell_is_empty(int x, int y) const { return brush_mask.cell_at_local_coord(x,y).tile_id == 0; }

        inline size_t num_hexes() const { return size().x * size().y; }
        size_t num_empty_hexes() const;
        inline size_t num_non_empty_hexes() const { return num_hexes() - num_empty_hexes(); }
    };

    terrain_brush_t terrain_brush_rectangle(int w, int y);
    terrain_brush_t terrain_brush_circle(float radius);
    terrain_brush_t terrain_brush_hexagon(int radius);
    //terrain_brush_t terrain_brush_from_bitmap(std::string const& filepath);


    std::vector<glm::ivec2> get_brush_grid_overlap(terrain_brush_t const& brush, hex_grid_t const& grid, glm::ivec2 grid_coord);
}

//TODO: move this to its own file
namespace ui
{
    struct terrain_brush_vertex_t
    {
        static gl_vertex_spec_<vertex_attrib::position3_t/*, vertex_attrib::color_t*/> vertex_spec;

        glm::vec3 position;
    };

    polygon_<terrain_brush_vertex_t> verts_for_terrain_brush(data::terrain_brush_t const&);
}
}
}