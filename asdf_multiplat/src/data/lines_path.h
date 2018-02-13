#pragma once

#include <vector>

#include <glm/glm.h>

#include "main/asdf_defs.h"

namespace asdf {
namespace data
{
    struct line_node_t
    {
        glm::vec3 position;
        color_t color;
    };

    struct bezier_spline_node_t : line_node_t
    {
        bezier_spline_node_t() = default;

        bezier_spline_node_t(glm::vec3 _pos, glm::vec3 _prev, glm::vec3 _next)
        : line_node_t{_pos}
        , curve_control_prev(_prev)
        , curve_control_next(_next)
        {}

        glm::vec3 curve_control_prev;
        glm::vec3 curve_control_next;
    };


    using polyline_t = std::vector<line_node_t>;
    using bezier_spline_t = std::vector<bezier_spline_node_t>;
}
}