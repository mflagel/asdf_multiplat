#include "stdafx.h"
#include "spline.h"

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
