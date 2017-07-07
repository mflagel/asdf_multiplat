#pragma once

#include <vector>
#include <set>
#include <memory>

#include <glm/glm.hpp>
#include <glm/core/type_half.hpp>
#include <glm/core/type_vec.hpp>

#include "asdf_multiplat/main/asdf_defs.h"
#include "asdf_multiplat/data/gl_vertex_spec.h"
#include "asdf_multiplat/ui/polygon.h"
#include "data/spline.h"

namespace asdf
{
    struct shader;

namespace vertex_attrib
{
    VERTEX_ATTRIB_NAMED(spline_vert_normal_, SplineNormal);
    using spline_vert_normal_t = spline_vert_normal_<1, float, GL_FLOAT>;

    VERTEX_ATTRIB_NAMED(extrusion_, VertexExtrusion);
    using extrusion_t = extrusion_<2, float, GL_FLOAT>;
}

namespace hexmap {
namespace ui
{
    using spline_vert_normal_t =  vertex_attrib::spline_vert_normal_t;

    struct spline_vertex_t
    {
        static gl_vertex_spec_<vertex_attrib::position2_t
                             , vertex_attrib::extrusion_t
                             , spline_vert_normal_t
                             , vertex_attrib::color_t> vertex_spec;

        glm::vec2 position;
        glm::vec2 extrusion;
        // just going to be 1 or -1, used for interpolation
        // Could optimize this and piggyback a single bit from another variable
        float normal; //int might make more sense but I'm not sure if there will be interpolation issues?
        color_t color;
    };


    struct spline_renderer_t
    {
        std::shared_ptr<shader_t> shader;
        rendered_multi_polygon_<spline_vertex_t> spline_geometry;  //opengl data (vao, vbo, etc)
        rendered_multi_polygon_<spline_vertex_t> handles_geometry; //FIXME probably doesn't need the extrusion vector from spline_vert

        //std::vector<data::spline_t const*> splines;
        std::vector<data::spline_t> const* spline_list;
        std::vector<size_t> spline_node_count_cache;
        std::set<size_t> dirty_splines;

        std::vector<std::vector<data::line_node_t>> reticulated_splines;

        void init(std::shared_ptr<shader_t>);

        void rebuild_spline(size_t spline_ind);
        void rebuild_all();
        bool rebuild_if_dirty();

        void render() const;
        void render_handles() const;
        void render_some_spline_handles(std::vector<size_t> spline_indices) const;


    };
}
}
}
