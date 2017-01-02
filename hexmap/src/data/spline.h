#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "asdf_multiplat/main/asdf_defs.h"
// #include "map_objects.h"

namespace asdf {
namespace hexmap {
namespace data
{
    struct line_node_t
    {
        glm::vec2 position;
        float thickness;
        color_t color;
    };

    using control_node_t = glm::vec2;

    struct spline_t
    {
        enum interpolation_e
        {
            linear
          , bezier
          // , cubic_bspline
          // , catmull_rom
          // , hermite
        };

        std::vector<line_node_t> nodes;
        std::vector<control_node_t> control_nodes;
        interpolation_e spline_type = linear;

        size_t size() const { return nodes.size(); }
    };


    struct spline_selection_t
    {
        std::vector<spline_t*> splines;
        std::vector<std::vector<size_t>> node_indices;
        bool is_control_node = false;

        void select_spline(spline_t&);
        void select_spline_nodes(spline_t&, std::vector<size_t> node_inds);
        void select_control_node(spline_t&, size_t control_node_index);

        void deselect_all();
    };
}
}
}
