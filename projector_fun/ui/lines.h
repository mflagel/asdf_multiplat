#pragma once

#include "polygon.h"

namespace asdf
{
namespace projector_fun
{

    struct line_point_t
    {
        enum join_type_e
        {
              join_none
            , join_miter
            , join_round
            , join_bevel
        };

        enum cap_type_e
        {
              cap_none
            , cap_round
            , cap_butt
            , cap_square
        };

        glm::vec3 position = vec3{0.0f};
        color_t color = COLOR_BLACK;
        float thickness = 1.0f;
    };

    struct line_vertex_t
    {
        glm::vec3 position;
        glm::vec3 normal;
    };

    struct line_t
    {
        using vertices_t = std::vector<line_vertex_t>;

        gl_renderable_t renderable;

        float feather = 0.5f;
        std::vector<line_point_t> line_points;

        vertices_t build_verts();
        void build_verts(vertices_t&, line_point_t const&, line_point_t const&);
        void build_verts(vertices_t&, line_point_t const& prev, line_point_t const& p, line_point_t const& next);
        void build_line_cap(vertices_t&, line_point_t const& prev, line_point_t const& p, line_point_t const& next);

        void render();
    };

}
}