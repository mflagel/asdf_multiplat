#include "stdafx.h"
#include "polygon_path.h"

#include <glm/gtx/spline.hpp>

using namespace std;
using namespace glm;

namespace asdf
{
namespace projector_fun
{
    polygon_path_state_t::polygon_path_state_t(ui_polygon_t& _polygon, bezier_polygon_path_t _path, size_t _max_loops)
    : polygon_state_t()
    , polygon(_polygon)
    , path(std::move(_path))
    , max_loops(_max_loops)
    {
        ASSERT(path.size() >= 2, "polygon path is just a single point");

        // LOG("--- Path ---");
        // for(auto node : path)
        // {
        //     LOG("%01f, %01f, %01f", node.x, node.y, node.z);
        // }
        // LOG("---");

        update_interpolator_nodes();
    }

    void polygon_path_state_t::update_interpolator_nodes()
    {
        interpolator.set_nodes(path[current_edge_index], path[current_edge_index + 1]);
    }

    void polygon_path_state_t::update(float dt)
    {
        interpolator.update(dt);

        if(interpolator.is_finished())
        {
            ++current_edge_index;

            //if at end of path
            if(current_edge_index >= path.size() - 1)
            {
                ++loop_count;
                if(max_loops != 0 && loop_count >= max_loops)
                {
                    //todo: exit state
                    return;
                }
                else
                {
                    current_edge_index = 0;
                }
            }

            update_interpolator_nodes();
        }

        polygon.position = interpolator.calc();
    }


    /// utility functions
    polygon_path_t polygon_path_from_deltas(glm::vec3 const& start, std::vector<glm::vec3> const& deltas, bool close_path)
    {
        polygon_path_t path;

        glm::vec3 current_position = start;
        path.push_back(current_position);

        for(auto const& delta : deltas)
        {
            current_position += delta;
            path.push_back(current_position);
        }

        if(close_path && path.back() != path.front())
        {
            path.push_back(path.front());
        }

        return path;
    }

    // each vertex creates bezier control points based on neighbouring vertices
    // draw a line between p{-1} and p{+1}. Apply half of that line in either direction
    // away from the current node
    bezier_polygon_path_t bezier_path_from_path(polygon_path_t const& _path, bool loops)
    {
        ASSERT(_path.size() > 2, "Can't bezier-ify a path with less than 3 nodes");

        bezier_polygon_path_t path;

        for(size_t i = 0; i < _path.size(); ++i)
        {
            bezier_path_node_t node{_path[i]};

            // size_t i_p = (ssize_t(i) - 1) % ssize_t(path.size());
            // size_t i_n = (i + 1 % path.size());
            size_t i_p = (i == 0)              ? (path.size() - 1) : (i - 1);
            size_t i_n = (i == path.size() -1) ? (0)               : (i + 1);

            node.curve_control_prev = _path[i_p];
            node.curve_control_next = _path[i_n];

            vec3 control_vec = node.curve_control_next - node.curve_control_prev;
            control_vec *= 0.25f; //todo: base the length of the control vec on the distance between the current vertex and the prev/next vertices

            node.curve_control_prev = node - control_vec;
            node.curve_control_next = node + control_vec;

            path.push_back(std::move(node));
        }

        if(!loops)
        {
            vec3 frontvec = (path[1] - path[0]) * 0.25f;
            path.front().curve_control_prev = vec3{0.0f};
            path.front().curve_control_next = vec3{-frontvec.y, frontvec.x, frontvec.z};

            vec3 backvec = (path[path.size()-1] - path[path.size()-2]) * 0.25f;
            path.back().curve_control_prev = vec3{-backvec.y, backvec.x, backvec.z};
            path.back().curve_control_next = vec3{0.0f};

            // path.front().curve_control_prev = vec3{0.0f};
            // path.front().curve_control_next = vec3{0.0f};
            // path.back(). curve_control_prev = vec3{0.0f};
            // path.back(). curve_control_next = vec3{0.0f};
        }

        return path;
    }

    // currently only generates linear connections
    // will implement curve part later
    polygon_t generate_bezier_path(bezier_polygon_path_t const& path, size_t num_facets_per_edge, bool show_control_points)
    {
        const color_t curve_color     = COLOR_LIGHTGREY;
        const color_t ctrl_prev_color = COLOR_RED;
        const color_t ctrl_next_color = COLOR_GREEN;
        color_t hull_color      = COLOR_CYAN;

        polygon_t polygon;

        for(size_t i = 0; i < path.size() - 1 || i == 0; ++i)
        {
            vec3 const& p0 = path[i];
            vec3 const& p1 = path[i].curve_control_next;
            vec3 const& p2 = path[i+1].curve_control_prev;
            vec3 const& p3 = path[i+1];

            // LOG("edge between {%0.3f, %0.3f} and {%0.3f, %0.3f}", p[0].x, p[0].y, p[3].x, p[3].y);
            // LOG("bezier curve for convex hull: {%0.3f, %0.3f}, {%0.3f, %0.3f}, {%0.3f, %0.3f}, {%0.3f, %0.3f}"
            //     , p0.x, p0.y
            //     , p1.x, p1.y
            //     , p2.x, p2.y
            //     , p3.x, p3.y);


            for(size_t i = 0; i < num_facets_per_edge - 1; ++i)
            {
                float t = (float)(i) / (float)(num_facets_per_edge - 1); //nasty casting
                glm::vec3 out1(bezier(p0, p1, p2, p3, t));

                // LOG("%0.3f, %0.3f   %f", out1.x, out1.y, t);

                      t = (float)(i+1) / (float)(num_facets_per_edge - 1); //nasty casting
                glm::vec3 out2(bezier(p0, p1, p2, p3, t));

                // LOG("%0.3f, %0.3f   %f", out2.x, out2.y, t);

                polygon.push_back(polygon_vertex_t{std::move(out1), curve_color});
                polygon.push_back(polygon_vertex_t{std::move(out2), curve_color});
            }

            //if(show_hull)
            {
                polygon.push_back(polygon_vertex_t{p0, hull_color});
                polygon.push_back(polygon_vertex_t{p1, hull_color});

                polygon.push_back(polygon_vertex_t{p1, hull_color});
                polygon.push_back(polygon_vertex_t{p2, hull_color});

                polygon.push_back(polygon_vertex_t{p2, hull_color});
                polygon.push_back(polygon_vertex_t{p3, hull_color});
            }
        }

        if(show_control_points)
        {
            for(auto const& vertex : path)
            {
                //prev
                polygon.push_back(polygon_vertex_t{vertex.curve_control_prev, ctrl_prev_color});
                polygon.push_back(polygon_vertex_t{vertex, ctrl_prev_color});

                //next
                polygon.push_back(polygon_vertex_t{vertex, ctrl_next_color});
                polygon.push_back(polygon_vertex_t{vertex.curve_control_next, ctrl_next_color});
            }
        }

        return polygon;
    }


}
}