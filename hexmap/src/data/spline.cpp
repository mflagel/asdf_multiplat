#include "stdafx.h"
#include "spline.h"

#include <glm/gtx/norm.hpp>

#include "asdf_multiplat/data/gl_vertex_spec.h"

namespace asdf {
    using namespace vertex_attrib;
namespace hexmap {
namespace data
{

    /* --Save Format--
        write the number of nodes (64-bit unsigned integer)
            write the nodes
        write the number of control nodes (64-bit unsigned integer)
            write the control nodes
    */
    void spline_t::save_to_file(SDL_RWops* io) const
    {
        ASSERT(io, "");
        {
            LOG("offset when writing spline node count: %zu", SDL_RWtell(io));
            uint64_t sz = nodes.size();
            SDL_RWwrite(io, reinterpret_cast<const void*>(&sz), sizeof(uint64_t), 1);
            LOG("wrote node count (%zu bytes)", sizeof(uint64_t));
            LOG("offset after writing spline node count: %zu", SDL_RWtell(io));
            
            size_t n = SDL_RWwrite(io, nodes.data(), sizeof(line_node_t), nodes.size());
            ASSERT(n == nodes.size(), "error writing spline_t::nodes");
            LOG("wrote %zu spline nodes (%zu bytes)", n, n * sizeof(line_node_t));
        }
        {
            uint64_t sz = control_nodes.size();
            SDL_RWwrite(io, reinterpret_cast<const void*>(&sz), sizeof(uint64_t), 1);
            LOG("wrote control_node count (%zu bytes)", sizeof(uint64_t));

            size_t n = SDL_RWwrite(io, control_nodes.data(), sizeof(control_node_t), control_nodes.size());
            ASSERT(n == control_nodes.size(), "error writing spline_t::control_nodes");
            LOG("wrote %zu control_nodes nodes (%zu bytes)", n, n * sizeof(control_node_t));
        }

        size_t sz = SDL_RWwrite(io, reinterpret_cast<const void*>(&spline_type), sizeof(interpolation_e), 1);
        ASSERT(sz == 1, "");
    }

    void spline_t::load_from_file(SDL_RWops* io)
    {
        ASSERT(io, "");
        {
            {
                LOG("offset when loading spline node count: %zu", SDL_RWtell(io));
                uint64_t sz = 0;
                size_t n = SDL_RWread(io, reinterpret_cast<void*>(&sz), sizeof(uint64_t), 1);
                ASSERT(n == 1, "Error writing spline node count");
                LOG("offset after loading spline node count: %zu", SDL_RWtell(io));

                nodes.resize(sz);
                n = SDL_RWread(io, nodes.data(), sizeof(line_node_t), sz);
                ASSERT(n == sz, "Error loading spline nodes");
            }
            {
                uint64_t sz =   0;
                SDL_RWread(io, reinterpret_cast<void*>(&sz), sizeof(uint64_t), 1);

                control_nodes.resize(sz);
                size_t n = SDL_RWread(io, control_nodes.data(), sizeof(line_node_t), sz);
                ASSERT(n == sz, "Error loading spline control nodes");
            }

            size_t n = SDL_RWread(io, reinterpret_cast<void*>(&spline_type), sizeof(interpolation_e), 1);
            ASSERT(n == 1, "Error reading spline interpolation type");
        }
    }


    bool point_intersects_spline(glm::vec2 const& world_pos, spline_t const& spline, float dist_threshold)
    {
        if(spline.nodes.size() == 0)
        {
            return false;
        }
        else if(spline.nodes.size() == 1)
        {
            auto dist = glm::length(spline.nodes[0].position - world_pos);
            return dist <= dist_threshold;
        }
        else
        {
            //get closest point to world_pos
            spline_node_index_t closest = 0;
            auto cv = spline.nodes[0].position - world_pos;
            auto closest_dist_sq = glm::length2(cv);

            for(size_t i = 1; i < spline.nodes.size(); ++i)
            {
                auto v = spline.nodes[i].position - world_pos;
                auto dsq = glm::length2(v);

                if(dsq < closest_dist_sq)
                {
                    closest = i;
                    closest_dist_sq = dsq;
                }
            }

            auto const& p0 = spline.nodes[closest].position;


            // get prev or next node, whichever is closest to world_pos (and exists)
            line_node_t const* prev = nullptr;
            line_node_t const* next = nullptr;

            if(spline.loops)
            {
                prev = (closest == 0) ? &(*spline.nodes.end())  : &(spline.nodes[closest-1]);
                next = (closest+1 == spline.nodes.size()) ?  &(spline.nodes[0])     : &(spline.nodes[closest+1]);
            }
            else
            {
                prev = (closest==0)                       ? nullptr : &(spline.nodes[closest-1]);
                next = (closest+1 == spline.nodes.size()) ? nullptr : &(spline.nodes[closest+1]);
            }

            ASSERT(next || prev, "somehow both next and prev are null");

            //if prev is null, use next, else use prev
            auto* second_closest = (prev != nullptr) ? prev : next;

            //if both are not-null, grab the closest one
            if(prev && next)
            {
                auto d_prev = glm::length2(prev->position - p0);
                auto d_next = glm::length2(next->position - p0);

                second_closest = (d_prev > d_next) ? next : prev;
            }


            auto p1 = second_closest->position;


            switch(spline.spline_type)
            {
                case spline_t::linear:
                    return circle_intersects_line(world_pos, dist_threshold, p0, p1);
                // case spline_t::bezier:
                {
                    //auto c1 = spline.control_nodes[2*inds[0]+0].position;
                    //auto c2 = spline.control_nodes[2*inds[0]+1].position;
                    //return circle_intersects_bezier(world_pos, dist_threshold
                    //                                n1.position, c1, c2, n2.position);
                }
                default: EXPLODE("TODO: implement intersection with %s splines", spline_interpolation_names[spline.spline_type]);
                    return false;
            }
        }
    }

    bool circle_intersects_line(glm::vec2 const& circle_pos, float radius, glm::vec2 const& p0, glm::vec2 const& p1)
    {
        

        return false;
    }

    bool circle_intersects_bezier(glm::vec2 circle_pos, float radius, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
    {
        EXPLODE("todo");
        return false;
    }


    void spline_selection_t::select_spline(spline_t& spline)
    {
        std::vector<size_t> new_inds;
        new_inds.reserve(spline.size());
        for(size_t i = 0; i < spline.size(); ++i)
        {
            new_inds.push_back(i);
        }

        select_spline_nodes(spline, std::move(new_inds));
    }

    void spline_selection_t::select_spline_nodes(spline_t& spline, std::vector<size_t> node_inds)
    {
        if(is_control_node)
        {
            deselect_all();
            is_control_node = false;
        }

        splines.push_back(&spline);
        node_indices.push_back(std::move(node_inds));
    }

    void spline_selection_t::select_control_node(spline_t& spline, size_t control_node_index)
    {
        deselect_all();
        is_control_node = true;

        splines.push_back(&spline);

        node_indices.resize(1);
        node_indices[0].push_back(control_node_index);
    }


    void spline_selection_t::deselect_all()
    {
        splines.clear();
        node_indices.clear();
    }
}
}
}
