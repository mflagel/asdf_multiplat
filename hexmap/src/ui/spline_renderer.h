#pragma once

#include <vector>
#include <set>
#include <memory>

#include <glm/glm.hpp>

#include "asdf_multiplat/main/asdf_defs.h"
#include "asdf_multiplat/ui/polygon.h"
#include "data/spline.h"

namespace asdf {
    struct shader;

namespace hexmap {
namespace ui
{
    struct spline_vertex_t
    {
        static gl_vertex_spec_<vertex_attrib::position3_t, vertex_attrib::color_t> vertex_spec;

        glm::vec3 position;
        color_t color;
    };

    struct spline_renderer_t
    {
        std::shared_ptr<shader_t> shader;
        rendered_multi_polygon_<spline_vertex_t> spline_geometry; //opengl data (vao, vbo, etc)
        rendered_multi_polygon_<spline_vertex_t> handles_geometry;

        //std::vector<data::spline_t const*> splines;
        std::vector<data::spline_t> const* spline_list;
        std::vector<size_t> spline_node_count_cache;
        std::set<size_t> dirty_splines;

        std::vector<std::vector<data::line_node_t>> reticulated_splines;

        void init(std::shared_ptr<shader_t>);

        void rebuild_spline(size_t spline_ind);
        void rebuild_all();
        bool rebuild_if_dirty();

        void render();
        void render_handles();
        void render_some_spline_handles(std::vector<size_t> spline_indices);


    };
}
}
}
