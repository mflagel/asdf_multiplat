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
        spline_polygon.initialize(shader);
    }

    void spline_renderer_t::batch(spline_t const& spline)
    {
        spline_batch.push_back(&spline);
    }

    void spline_renderer_t::batch(std::vector<spline_t> const& splines)
    {
        spline_batch.reserve(spline_batch.size() + splines.size());

        for(size_t i = 0; i < splines.size(); ++i)
        {
            spline_batch.push_back(&(splines[i]));
        }
    }

    void reticulated_splines()
    {
    }

    ///FIXME optimize to not reticulate every single frame
    void spline_renderer_t::end()
    {
        if(spline_batch.empty())
        {
            return;
        }

        ASSERT(shader, "cannot render splines without a shader");

        /// reticulate splines
        std::vector<std::vector<line_node_t>> constructed_lines;
        constructed_lines.reserve(spline_batch.size());

        for(auto const* spline : spline_batch)
        {
            ASSERT(spline, "null spline in batch");
            if(spline->size() > 1)
            {
                constructed_lines.push_back( line_from_interpolated_spline(*spline, default_subdivs_per_spline_segment) );
            }
            
        }

        constructed_lines.shrink_to_fit();

        if(constructed_lines.empty())
        {
            return;
        }


        //pre-loop to get size info
        size_t num_verts = 0;
        for(auto const& polyline : constructed_lines)
        {
            //not doing thickness yet, so only one vertex per polyline node
            num_verts += polyline.size();
        }

        /// set up renderable vertices
        polygon_<spline_vertex_t> verts;
        verts.resize(num_verts);

        size_t vert_ind = 0;
        for(auto const& polyline : constructed_lines)
        {
            for(auto const& line_vert : polyline)
            {
                verts[vert_ind].position = glm::vec3(line_vert.position, 0.0f);
                verts[vert_ind].color    = line_vert.color;

                ++vert_ind;
            }
        }

        ASSERT(vert_ind == num_verts, "Unexpected unset vertices in polyline");

        /// Render
        GL_State->bind(shader);
        shader->update_wvp_uniform();

        spline_polygon.set_data(verts);
        spline_polygon.render(GL_LINE_LOOP); //will change this to GL_TRIANGLES later when I implement thickness
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
                auto t = static_cast<float>( (i+1) / (subdivisions_per_segment+2) ); //i+1 becuase the 0th node is the spline's start node, not this one
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