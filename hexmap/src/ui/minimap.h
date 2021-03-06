#pragma once

#include <glm/glm.hpp>

#include <asdfm/data/gl_resources.h>
#include <asdfm/data/gl_vertex_spec.h>

#include "ui/hex_map.h"

namespace asdf
{
namespace hexmap
{
namespace ui
{
    /*  Minimap Strategy

        - each cell will be rendered as a 1x1 unit quad
            these quads will be single-colored non-textured
        - every second row will be rendered 0.5 units lower
        - TODO: render map_objects and splines?
            Might make this toggle-able
        - the minimap geometry will then be rendered to a texture
        - a rectangle with a 1px stroke will be rendered on top to 
          display the currently viewed area
    */

    struct minimap_vertex_t
    {
        static gl_vertex_spec_<vertex_attrib::position2_t
                             , vertex_attrib::color_t> vertex_spec;

        glm::vec2 position;
        glm::vec4 color;
    };

    struct minimap_t
    {
        ui::hex_map_t& rendered_map;
        data::hex_map_t const& map_data;

        render_target_t render_target;

        // rendered_multi_polygon_<minimap_vertex_t> minimap_geometry;

        minimap_t(ui::hex_map_t&);

        void rebuild();

        void render();
    };
}
}
}