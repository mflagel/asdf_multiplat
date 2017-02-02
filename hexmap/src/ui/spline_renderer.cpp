#include "stdafx.h"
#include "spline_renderer.h"

#include <glm/gtx/spline.hpp>
#include <glm/gtx/compatibility.hpp> //for lerp

#include "asdf_multiplat/utilities/utilities.h"
#include "asdf_multiplat/data/gl_state.h"

namespace asdf {
namespace hexmap
{
    using namespace data;

namespace ui
{
    namespace
    {
        constexpr size_t default_subdivs_per_spline_segment = 10;
    }

    /// vertex spec allocation / definition
    gl_vertex_spec_<vertex_attrib::position3_t, vertex_attrib::color_t> spline_vertex_t::vertex_spec;

    /// Helper Func Declarations
    line_node_t interpolated_node(spline_t const& spline, size_t spline_node_ind, float t);
    std::vector<line_node_t> line_from_interpolated_spline(spline_t const& spline, size_t subdivisions_per_segment);
    line_node_t interpolate_linear(line_node_t const& start, line_node_t const& end, float t);
    line_node_t interpolate_bezier(line_node_t const& start, control_node_t const& cn_start, control_node_t const& cn_end, line_node_t const& end, float t);


    /// Member Func Definitions
    void spline_renderer_t::init(std::shared_ptr<shader_t> _shader)
    {
        shader = std::move(_shader);
        spline_geometry.initialize(shader);
    }

    // void spline_renderer_t::batch(spline_t const& spline)
    // {
    //     spline_batch.push_back(&spline);
    // }

    // void spline_renderer_t::batch(std::vector<spline_t> const& splines)
    // {
    //     spline_batch.reserve(spline_batch.size() + splines.size());

    //     for(size_t i = 0; i < splines.size(); ++i)
    //     {
    //         spline_batch.push_back(&(splines[i]));
    //     }
    // }

    void spline_renderer_t::rebuild_spline(size_t spline_ind)
    {
        //TODO
    }

    void spline_renderer_t::rebuild_all()
    {
        if(!spline_list || (spline_list && spline_list->empty()))
            return;

        reticulated_splines.clear();

        /// reticulate splines
        auto& constructed_lines = reticulated_splines;             // I am lazy and aliasing the variable name
        //std::vector<std::vector<line_node_t>> constructed_lines; // instead of find/replace
        constructed_lines.reserve(spline_list->size());

        for(auto const& spline : *spline_list)
        {
            if(spline.size() > 1) //ignore splines with less than two points (and thus no segments)
            {
                constructed_lines.push_back( line_from_interpolated_spline(spline, default_subdivs_per_spline_segment) );
            }
            
        }

        constructed_lines.shrink_to_fit();

        if(constructed_lines.empty())
        {
            return;
        }

        /// set up renderable vertices
        std::vector<polygon_<spline_vertex_t>> rendered_vertex_lists;
        rendered_vertex_lists.resize(constructed_lines.size());

        for(size_t i = 0; i < constructed_lines.size(); ++i)
        {
            rendered_vertex_lists[i].resize(constructed_lines[i].size());

            for(size_t vert_ind = 0; vert_ind < constructed_lines[i].size(); ++vert_ind)
            {
                auto& rvert = rendered_vertex_lists[i][vert_ind];
                auto& cvert = constructed_lines[i][vert_ind];

                rvert.position = glm::vec3(cvert.position, 0.0f);
                rvert.color    = cvert.color;
            }
        }

        spline_geometry.set_data(rendered_vertex_lists);
    }

    bool spline_renderer_t::rebuild_if_dirty()
    {
        if(!spline_list)
        {
            return false;
        }
        else if(spline_list->size() != spline_node_count_cache.size())
        {
            rebuild_all();
        }

        //update spline node count cache and rebuild splines if dirty
        //TODO: optimize to only rebuild dirty splines instead of all of them
        bool dirty = false;
        spline_node_count_cache.resize(spline_list->size(), 0); //init new elements as 0
        for(size_t i = 0; i < spline_list->size(); ++i)
        {
            dirty |= ((*spline_list)[i].size() != spline_node_count_cache[i]);
            // if((*spline_list)[i].size() != spline_node_count_cache[i])
            //     dirty_splines.insert(i);

            spline_node_count_cache[i] = (*spline_list)[i].size();
        }

        //TEMP
        dirty |= dirty_splines.size() > 0;
        dirty_splines.clear();
        //

        if(dirty)
        {
            rebuild_all();
        }

        return true;
    }

    void spline_renderer_t::render()
    {
        ASSERT(shader, "cannot render splines without a shader");

        //if after rebuilding, there is nothing to render, don't bother with anything below
        if(spline_geometry.num_sub_meshes() == 0)
        {
            return;
        }

        GL_State->bind(shader);
        shader->update_wvp_uniform();

        spline_geometry.render(GL_LINE_STRIP); //will change this to GL_TRIANGLES later when I implement thickness
    }


    }


    /// Helper Func Definitions
    line_node_t interpolated_node(spline_t const& spline, size_t spline_node_ind, float t)
    {
        ASSERT(spline.nodes.size() > spline_node_ind, "out of bounds");

        switch(spline.spline_type)
        {
            case spline_t::linear:
            {
                return interpolate_linear(spline.nodes[spline_node_ind], spline.nodes[spline_node_ind+1], t);
            }
            case spline_t::bezier:
            {
                auto cn_ind = spline_node_ind * 2;

                ASSERT(spline.control_nodes.size() > cn_ind, "out of bounds");

                auto const& p0 = spline.nodes[spline_node_ind];
                auto const& p1 = spline.control_nodes[cn_ind];
                auto const& p2 = spline.control_nodes[cn_ind + 1];
                auto const& p3 = spline.nodes[spline_node_ind+1];

                return interpolate_bezier(p0, p1, p2, p3, t);
            }
        };

        return spline.nodes[spline_node_ind];
    }


    std::vector<line_node_t> line_from_interpolated_spline(spline_t const& spline, size_t subdivisions_per_segment)
    {
        std::vector<line_node_t> constructed_line;

        if(spline.size() == 0 || spline.spline_type == spline_t::linear)
            return spline.nodes;

        //space for the original nodes plus subdivisions
        constructed_line.reserve(spline.size() + subdivisions_per_segment * spline.size() - 1);

        for(size_t spline_node_ind = 0; spline_node_ind < spline.size() - 1; ++spline_node_ind)
        {
            //start node
            constructed_line.push_back(spline.nodes[spline_node_ind]);

            for(size_t i = 0; i < subdivisions_per_segment; ++i)
            {
                auto t = float(i+1) / float(subdivisions_per_segment+2); //i+1 becuase the 0th node is the spline's start node, not this one
                                                                         //subdivisions_per_segment+2 because 
                constructed_line.push_back(interpolated_node(spline, spline_node_ind, t));
            }

            //end node
            constructed_line.push_back(spline.nodes[spline_node_ind+1]);
        }


        //todo: simpilfy spline by removing nodes that are relatively coplanar


        return constructed_line;
    }

    line_node_t interpolate_linear(line_node_t const& start, line_node_t const& end, float t)
    {
        line_node_t node;
        node.position  = glm::lerp(start.position,  end.position,  t);
        node.thickness = glm::lerp(start.thickness, end.thickness, t);
        node.color     = glm::lerp(start.color,     end.color,     t);

        return node;
    }

    line_node_t interpolate_bezier(line_node_t const& start, control_node_t const& cn_start, control_node_t const& cn_end, line_node_t const& end, float t)
    {
        line_node_t node;
        node.position = bezier(start.position, cn_start, cn_end, end.position, t);

        //TODO: do something non-linear for interpolating non-position attributes?
        node.thickness = glm::lerp(start.thickness, end.thickness, t);
        node.color = glm::lerp(start.color, end.color, t);

        return node;
    }
}
}
}