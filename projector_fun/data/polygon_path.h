#pragma once;

#include "polygon.h"
#include /*"asdf/*/"interpolator.h"
#include "polygon_state_machine.h"

#include <glm/gtx/spline.hpp>

namespace asdf
{
namespace projector_fun
{

    using path_node_t = glm::vec3; // position

    struct bezier_path_node_t : path_node_t
    {
        using path_node_t::path_node_t; //required to construct bezier_path_node_t from a vec3

        bezier_path_node_t() = default;

        bezier_path_node_t(glm::vec3 _pos, glm::vec3 _prev, glm::vec3 _next)
        : path_node_t(_pos)
        , curve_control_prev(_prev)
        , curve_control_next(_next)
        {}

        glm::vec3 curve_control_prev;
        glm::vec3 curve_control_next;
    };

    template <class T>
    struct movement_interpolator_ : interpolator_<T>
    {
        static constexpr float movement_speed = 400.0f; //pixels per second. will probably make this variable at some point

        movement_interpolator_() = default;

        movement_interpolator_(T _start, T _dest)
        {
            set_nodes(std::move(_start), std::move(_dest));
        }

        void set_nodes(T _start, T _dest)
        {
            interpolator_<T>::reset_smooth(); //must do this before total_time changes

            interpolator_<T>::start = std::move(_start);
            interpolator_<T>::dest  = std::move(_dest);
            interpolator_<T>::total_time = glm::length(interpolator_<T>::dest - interpolator_<T>::start) / movement_speed;
        }
    };

    using movement_interpolator_t = movement_interpolator_<path_node_t>;

    struct bezier_movement_interpolator_t : movement_interpolator_<bezier_path_node_t>
    {
        using movement_interpolator_<bezier_path_node_t>::movement_interpolator_;

        glm::vec3 calc() const
        {
            float t = get_percentage();

            std::array<glm::vec3, 4> p{
                  this->start
                , this->start.curve_control_next
                , this->dest.curve_control_prev
                , this->dest
            };

            //cubic curve
            return bezier(p[0], p[1], p[2], p[3], t);
        }
    };

    struct spline_movement_interpolator_t : movement_interpolator_t
    {
        path_node_t pre;
        path_node_t post;

        void set_nodes(path_node_t _pre, path_node_t _start, path_node_t _dest, path_node_t _post)
        {
            movement_interpolator_t::set_nodes(std::move(_start), std::move(_dest));

            pre  = std::move(_pre);
            post = std::move(_post);
        }

        path_node_t calc() const
        {
            float t = get_percentage();

            return glm::cubic(pre, start, dest, post, t); //cubic b-spline
        }
    };


    using polygon_list_t = std::vector<ui_polygon_t>;

    template <class N> // requires N descends from path_node_t
    using polygon_path_ = std::vector<N>;
    using polygon_path_t = polygon_path_<path_node_t>;
    using bezier_polygon_path_t = polygon_path_<bezier_path_node_t>;

    struct polygon_path_state_t : polygon_state_t
    {
        ui_polygon_t& polygon;

        bezier_polygon_path_t path;
        size_t current_edge_index = 0; //current edge consists of nodes {path[current_edge_index], path[current_edge_index+1]}

        // movement_interpolator_t interpolator;
        bezier_movement_interpolator_t interpolator;

        size_t loop_count = 0;
        size_t max_loops = 0; //0 for infinite loops

        polygon_path_state_t(ui_polygon_t&, bezier_polygon_path_t, size_t _max_loops = 0);

        void update_interpolator_nodes();

        virtual void update(float dt) override;
    };

    // utility functions
    polygon_path_t polygon_path_from_deltas(glm::vec3 const& start, std::vector<glm::vec3> const& deltas, bool close_path = false);
    bezier_polygon_path_t bezier_path_from_path(polygon_path_t const&, bool loops = true);
    
    polygon_t generate_bezier_path(bezier_polygon_path_t const&, size_t num_facets_per_edge = 20, bool show_control_points = false);
    
}
}