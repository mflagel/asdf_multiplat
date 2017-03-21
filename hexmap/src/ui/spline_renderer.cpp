#include "stdafx.h"
#include "spline_renderer.h"

#include <algorithm>
#include <glm/gtx/spline.hpp>
#include <glm/gtx/compatibility.hpp> //for lerp

#include "asdf_multiplat/utilities/utilities.h"
#include "asdf_multiplat/data/gl_state.h"

using namespace glm;

namespace asdf {
namespace hexmap
{
    using namespace data;

namespace ui
{
    namespace
    {
        constexpr size_t default_subdivs_per_spline_segment = 10;
        constexpr float miter_limit = 1.0f;

        constexpr uint32_t spline_handle_size_px = 10;
        const/*expr*/ color_t spline_handle_color = color_t(1.0f, 1.0f, 1.0f, 1.0f);
    }

    /// vertex spec allocation / definition
    gl_vertex_spec_<vertex_attrib::position2_t
                  , vertex_attrib::extrusion_t
                  , spline_vert_normal_t
                  , vertex_attrib::color_t>    spline_vertex_t::vertex_spec;


    /// Helper Func Declarations
    line_node_t interpolated_node(spline_t const& spline, size_t spline_node_ind, float t);
    std::vector<line_node_t> line_from_interpolated_spline(spline_t const& spline, size_t subdivisions_per_segment);
    line_node_t interpolate_linear(line_node_t const& start, line_node_t const& end, float t);
    line_node_t interpolate_bezier(line_node_t const& start, control_node_t const& cn_start, control_node_t const& cn_end, line_node_t const& end, float t);

    std::vector<polygon_<spline_vertex_t>> build_spline_handles(spline_t const& spline);



    /*
    Current (Poly)Line Rendering
        On Geometry Rebuild:
            Allocate a bunch of GPU memory with a VBO
                Currently one vertex per line node
            Allocate main memory array of spline_vertex_t
            Copy relevant data from line_nodes to spline_vertex_t array
            Push spline_vertex_t array from main memory to VBO
        Render VBO using GL_LINE_STRIP and whatever thickness was set before

        Notes: Could potentially skip the local array of spline_vertex_t
               and copy directly from line_node_t?


    Current Spline Reticulation:
        Reticulate a spline by subdividing each spline segment
            Each newly create node has its position calculated with the desired interpolation
        TODO: simplify coplanar vertices

        Spline is then rendered the same as a polyline


    New Method:
        Line Reticulation:
            Each segment generates 4 vertices to form a quad:
              1 +-------------+ 3
                |``''--.._    |
              0 +----------``-+ 2
            However vertices for the starts and ends of lines will
            originally be placed in the same position
            They will be transformed in the vertex shader by moving them
            along a normal vector. The distance moved is half the line
            thickness
    */



    /// Member Func Definitions
    void spline_renderer_t::init(std::shared_ptr<shader_t> _shader)
    {
        shader = std::move(_shader);
        spline_geometry.initialize(shader);
        handles_geometry.initialize(shader);
    }

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

        size_t vert_list_index = 0;
        for(auto const& constructed_line : constructed_lines)
        {
            ASSERT(constructed_line.size() > 1, "constructed line is empty, or only has one point");
            size_t num_segments = constructed_line.size() - 1;
            rendered_vertex_lists[vert_list_index].resize(num_segments * 4); //4 verts (a quad) per line segment

            for(size_t segment_index = 0; segment_index < num_segments; ++segment_index)
            {
                //   layout of quad (draw using triangle_strip)
                // 1 +-------------+ 3
                //   |``''--.._    |
                // 0 +----------``-+ 2

                auto const& start_node = constructed_line[segment_index];
                auto const& end_node   = constructed_line[segment_index + 1];

                auto* verts = rendered_vertex_lists[vert_list_index].data() + (segment_index * 4);
                verts[0].position = start_node.position;
                verts[1].position = start_node.position;
                verts[2].position = end_node.position;
                verts[3].position = end_node.position;

                /// extrusion vector  TODO: support joints
                vec2 segment_vector = end_node.position - start_node.position;
                segment_vector.x += 0.0000001f * (segment_vector.x == 0.0f && segment_vector.y == 0.0f);
                segment_vector = glm::normalize(segment_vector);

                verts[0].extrusion = vec2(segment_vector.y, -segment_vector.x); //rotate seg_vec forward  90 degrees
                verts[1].extrusion = vec2(-segment_vector.y, segment_vector.x); //rotate seg_vec backward 90 degrees
                verts[2].extrusion = verts[0].extrusion;
                verts[3].extrusion = verts[1].extrusion;

                // bake thickness into extrusion vector
                auto s_thc = start_node.thickness / 2.0f;
                auto e_thc =   end_node.thickness / 2.0f;
                verts[0].extrusion *= s_thc;
                verts[1].extrusion *= s_thc;
                verts[2].extrusion *= e_thc;
                verts[3].extrusion *= e_thc;

                //handle edge joints with previous segment
                if(segment_index > 0)
                {
                    auto* prev_verts = rendered_vertex_lists[vert_list_index].data() + ((segment_index - 1) * 4);

                    //TODO: implement miter limit

                    //just add verts together to get correct direciton
                    auto top_extr = prev_verts[3].extrusion + verts[1].extrusion;
                    auto btm_extr = prev_verts[2].extrusion + verts[0].extrusion;
                    
                    prev_verts[3].extrusion = top_extr;
                    verts[1].extrusion = top_extr;

                    prev_verts[2].extrusion = btm_extr;
                    verts[0].extrusion = btm_extr;
                }

                /// normal (1 or -1, used to interpolate in fragment shader and get distance to the line center
                verts[0].normal = -1.0f;
                verts[1].normal =  1.0f;
                verts[2].normal = -1.0f;
                verts[3].normal =  1.0f;

                verts[0].color = start_node.color;
                verts[1].color = start_node.color;
                verts[2].color = end_node.color;
                verts[3].color = end_node.color;
            }

            ++vert_list_index;
        }

        spline_geometry.set_data(rendered_vertex_lists);


        std::vector<polygon_<spline_vertex_t>> handles_vert_lists;
        for(auto const& spline : *spline_list)
        {
            auto handles = build_spline_handles(spline);
            handles_vert_lists.insert(std::end(handles_vert_lists), std::begin(handles), std::end(handles));
        }


        handles_geometry.set_data(handles_vert_lists);
    }


    ///TODO: better handle tracking of what to return
    ///      chunks of this function could probably be organized better
    bool spline_renderer_t::rebuild_if_dirty()
    {
        bool rebuilt = false;

        if(!spline_list)
        {
            return false;
        }
        else if(spline_list->size() != spline_node_count_cache.size())
        {
            rebuild_all();
            rebuilt = true;
        }
        else if(spline_list->empty())
        {
            spline_geometry.first_vert_indices.clear(); //reset this so that no splines are rendered after deleting/removing all splines
            spline_geometry.vert_counts.clear();
            return true;  //true, since the geometry was 'rebuilt' to be empty
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
            rebuilt = true;
        }

        return rebuilt;
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

        spline_geometry.render(GL_TRIANGLE_STRIP);

        ASSERT(!CheckGLError(), "Error in spline_renderer_t::render()");
    }

    ///TODO: factor out similarities between this and the above render func?
    void spline_renderer_t::render_handles()
    {
        ASSERT(shader, "");

        if(handles_geometry.num_sub_meshes() == 0)
        {
            return;
        }

        GL_State->bind(shader);
        shader->update_wvp_uniform();
        
        handles_geometry.render(GL_LINE_STRIP);
    }

    void spline_renderer_t::render_some_spline_handles(std::vector<size_t> spline_indices)
    {
        std::vector<GLint> first_vert_indices;
        std::vector<GLsizei> vert_counts;

        size_t current_spline_ind = 0;
        size_t current_primative_index = 0;

        std::sort(spline_indices.begin(), spline_indices.end()); //should I just require the list to be pre-sorted?

        for(auto const& spline_ind_to_render : spline_indices)
        {
            // because each spline has a variable number of nodes, and a variable number of
            // handle primatives per node, count upwards to get the primative_index of the
            // spline to be rendered
            // OPTIMIZE: cache this
            for(; current_spline_ind < spline_ind_to_render; ++current_spline_ind)
            {
                auto const& cur_spline = (*spline_list)[current_spline_ind];
                current_primative_index += cur_spline.nodes.size() * handle_primatives_per_node[cur_spline.spline_type];
            }

            auto const& spline = (*spline_list)[spline_ind_to_render];

            //grab the subset of handles_geometry.first_vert_indices and .vert_counts corrisponding to this spline
            size_t primatives_subset_index = current_primative_index;
            size_t num_handle_prims = spline.nodes.size() * handle_primatives_per_node[spline.spline_type];
            size_t primatives_subset_end_index = current_primative_index + num_handle_prims;

            first_vert_indices.insert(first_vert_indices.begin()
                                    , handles_geometry.first_vert_indices.begin() + primatives_subset_index
                                    , handles_geometry.first_vert_indices.begin() + primatives_subset_end_index);

            vert_counts.insert(vert_counts.begin()
                             , handles_geometry.vert_counts.begin() + primatives_subset_index
                             , handles_geometry.vert_counts.begin() + primatives_subset_end_index);
        }

        GL_State->bind(handles_geometry.vao);
        size_t num_polygons = first_vert_indices.size();
        glMultiDrawArrays(GL_LINE_STRIP, first_vert_indices.data(), vert_counts.data(), num_polygons);
        GL_State->unbind_vao();
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
                auto cn_ind = spline_node_ind * 2 + 1;  //+1 so it grabs the ahead cnode of the start node, and the behind cnode of the second node

                ASSERT(spline.control_nodes.size() > cn_ind + 1, "out of bounds");

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


    std::vector<polygon_<spline_vertex_t>> build_spline_handles(spline_t const& spline)
    {
        /// FIXME do proper px->world conversion
        float spline_handle_size_units = float(spline_handle_size_px) / 100.0f;
        float sh_s_d2 = spline_handle_size_units / 2.0f;


        std::vector<polygon_<spline_vertex_t>> vertex_lists;

        for(auto const& node : spline.nodes)
        {
            vertex_lists.emplace_back();
            auto& vert_list = vertex_lists.back(); //in c++17 I can just grab what emplace_back() returns

            vert_list.push_back(spline_vertex_t{node.position, vec2{}, 0.0f, spline_handle_color});
            vert_list.push_back(spline_vertex_t{node.position, vec2{}, 0.0f, spline_handle_color});
            vert_list.push_back(spline_vertex_t{node.position, vec2{}, 0.0f, spline_handle_color});
            vert_list.push_back(spline_vertex_t{node.position, vec2{}, 0.0f, spline_handle_color});

            //square shape
            vert_list[0].position += vec2(-sh_s_d2,  sh_s_d2); //top left
            vert_list[1].position += vec2( sh_s_d2,  sh_s_d2); //top right
            vert_list[2].position += vec2( sh_s_d2, -sh_s_d2); //bottom right
            vert_list[3].position += vec2(-sh_s_d2, -sh_s_d2); //bottom left
        }

        if(spline.spline_type == spline_t::bezier)
        {
            auto push_handle_verts = [&sh_s_d2](polygon_<spline_vertex_t>& vert_list, vec2 pos)
            {
                vert_list.push_back(spline_vertex_t{pos,  vec2{}, 0.0f, spline_handle_color});
                vert_list.push_back(spline_vertex_t{pos,  vec2{}, 0.0f, spline_handle_color});
                vert_list.push_back(spline_vertex_t{pos,  vec2{}, 0.0f, spline_handle_color});
                vert_list.push_back(spline_vertex_t{pos,  vec2{}, 0.0f, spline_handle_color});

                //diamond shape
                vert_list[0].position.y += sh_s_d2; //up
                vert_list[1].position.x += sh_s_d2; //right
                vert_list[2].position.y -= sh_s_d2; //down
                vert_list[3].position.x -= sh_s_d2; //left
            };

            //for every group of two control nodes
            for(size_t i = 0; i < spline.control_nodes.size(); i += 2)
            {
                auto const& cnode_1 = spline.control_nodes[i];
                vertex_lists.emplace_back();
                push_handle_verts(vertex_lists.back(), cnode_1);

                auto const& cnode_2 = spline.control_nodes[i+1];
                vertex_lists.emplace_back();
                push_handle_verts(vertex_lists.back(), cnode_2);

                //connecting line
                vertex_lists.emplace_back();
                vertex_lists.back().push_back(spline_vertex_t{cnode_1,  vec2{}, 0.0f, spline_handle_color});
                vertex_lists.back().push_back(spline_vertex_t{cnode_2,  vec2{}, 0.0f, spline_handle_color});
            }
        }

        return vertex_lists;
    }
}
}
}