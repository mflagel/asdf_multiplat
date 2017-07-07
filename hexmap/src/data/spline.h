#pragma once

#include <vector>
#include <array>

#include <glm/glm.hpp>

#include "asdf_multiplat/main/asdf_defs.h"
// #include "map_objects.h"

struct SDL_RWops;

namespace asdf {
namespace hexmap {
namespace data
{
    using spline_index_t = size_t;
    using spline_node_index_t = size_t;

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
            , cubic_bspline
            , catmull_rom
            , hermite
            , bezier
            , num_interp_types
        };

        std::vector<line_node_t> nodes;
        std::vector<control_node_t> control_nodes;
        interpolation_e spline_type = linear;
        bool loops = false;

        size_t size() const { return nodes.size(); }

        void save_to_file(SDL_RWops*) const;
        void load_from_file(SDL_RWops*);
    };

    bool point_intersects_spline(glm::vec2 const& world_pos, spline_t const&, float dist_threshold = 0.1f);

    bool circle_intersects_line(glm::vec2 const& circle_pos, float radius, glm::vec2 const& p0, glm::vec2 const& p1);
    bool circle_intersects_bezier(glm::vec2 const& circle_pos, float radius
                                , glm::vec2 const& p0, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec2 const& p3);

    constexpr std::array<const char*, spline_t::num_interp_types> spline_interpolation_names =
    {
          "linear"
        , "cubic_bspline"
        , "catmull_rom"
        , "hermite"
        , "bezier"
    };

    constexpr std::array<size_t, spline_t::num_interp_types> handle_primatives_per_node
    {
          1 // linear
        , 0 // cubic_bspline
        , 0 // catmull_rom
        , 0 // hermite
        , 4 // bezier
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
