#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include <asdfm/ui/polygon.h>
#include <asdfm/data/shader.h>

#include "data/terrain_brush.h"

namespace asdf
{
namespace hexmap
{
namespace ui
{
    struct terrain_brush_vertex_t
    {
        static gl_vertex_spec_<vertex_attrib::position3_t> vertex_spec;

        glm::vec3 position;
    };

    std::vector<polygon_<terrain_brush_vertex_t>> verts_for_terrain_brush(data::terrain_brush_t const&);

    struct terrain_brush_renderer_t
    {
        std::shared_ptr<shader_t> shader;
        rendered_multi_polygon_<terrain_brush_vertex_t> brush_geometry;

        data::terrain_brush_t* brush = nullptr;

        terrain_brush_renderer_t() = default;

        void init(std::shared_ptr<shader_t>);

        void set_brush(data::terrain_brush_t*);
        void rebuild_geometry();

        void render();
    };
}
}
}