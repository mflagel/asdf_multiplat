#include "stdafx.h"
#include "hex_map.h"

#include <glm/gtx/norm.hpp>

namespace asdf
{
namespace hexmap
{
namespace data
{
    constexpr const char* default_map_name = "unnamed map";
    constexpr size_t hxm_version = 1;

    hex_map_t::hex_map_t(std::string const& _map_name, glm::uvec2 grid_size, hex_grid_cell_t const& default_cell_style)
    : map_name(_map_name)
    , hex_grid(grid_size, default_cell_style)
    {

    }

    hex_map_t::hex_map_t(glm::uvec2 grid_size)
    : hex_map_t(default_map_name, grid_size)
    {
    }

    //TODO: refactor save/load code such that hex_grid_t::save_to_file() takes a SDL_RWops*
    //      to read/write to

    // https://wiki.libsdl.org/SDL_RWops?highlight=%28%5CbCategoryStruct%5Cb%29%7C%28CategoryIO%29
    // https://wiki.libsdl.org/SDL_RWwrite?highlight=%28%5CbCategoryIO%5Cb%29%7C%28CategoryEnum%29%7C%28CategoryStruct%29
    void hex_map_t::save_to_file(std::string const& filepath)
    {
        SDL_RWops* io = SDL_RWFromFile(filepath.c_str(), "wb");

        if(!io)
        {
            EXPLODE("data::hex_map_t::save_to_file() error");  //todo: handle errors better
        }

        hxm_header_t header;
        header.version = 1;
        header.chunk_size = hex_grid.chunk_size();
        header.map_size = hex_grid.size;
        header.num_map_objects = objects.size();
        header.num_splines = splines.size();

        size_t num_written = SDL_RWwrite(io, reinterpret_cast<const void*>(&header), sizeof(hxm_header_t), 1);

        if(num_written != 1)
        {
            EXPLODE("error writing hxm header");
        }
        else
        {
            LOG("wrote header (%zu bytes)", sizeof(hxm_header_t));
        }

        /// save data
        hex_grid.save_to_file(io);
        LOG("offset after hex_grid: %zu", SDL_RWtell(io));

        {
            num_written = SDL_RWwrite(io, objects.data(), sizeof(map_object_t), objects.size());
            ASSERT(num_written == objects.size(), "error saving map_objects");
            LOG("wrote %zu map_objects (%zu bytes)", num_written, num_written * sizeof(map_object_t));
            LOG("offset after objects: %zu", SDL_RWtell(io));
        }


        for(auto const& spline : splines)
        {
            spline.save_to_file(io);
        }
        LOG("offset after splines: %zu", SDL_RWtell(io));

        LOG("total stream size: %zu", SDL_RWtell(io));

        /// close file and commit to disk
        SDL_RWclose(io);
    }

    void hex_map_t::load_from_file(std::string const& filepath)
    {
        SDL_RWops* io = SDL_RWFromFile(filepath.c_str(), "rb");

        if (!io)
        {
            EXPLODE("data::hex_map_t::load_to_file()");  //todo: handle errors better
        }


        hxm_header_t header;
        size_t num_read = SDL_RWread(io, &header, sizeof (hxm_header_t), 1);
        ASSERT(num_read > 0, "Error reading file header");

        ASSERT(header.version == hxm_version, "incorrect hxm version (got %zu, expected %zu)", header.version, hxm_version);

        ///load data
        hex_grid.load_from_file(io, header);
        LOG("offset after grid: %zu", SDL_RWtell(io));

        {
            //objects.reserve(header.num_map_objects);
            uint64_t n = SDL_RWread(io, objects.data(), sizeof (map_object_t), objects.size());
            ASSERT(n == objects.size(), "Error reading map objects");
            LOG("read %zu map objects", n);
        }
        LOG("offset after objects: %zu", SDL_RWtell(io));


        splines.resize(header.num_splines, spline_t{});
        for(auto& spline : splines)
        {
            spline.load_from_file(io);
        }
        LOG("offset after splines: %zu", SDL_RWtell(io));

        LOG("total stream size: %zu", SDL_RWtell(io));

        SDL_RWclose(io);
    }

    ///OPTIMIZE: iterate from end to start and grab the first one that intersects
    size_t hex_map_t::object_index_at(glm::vec2 const& world_pos) const
    {
        auto possible_object_indices = object_indices_at(world_pos);
        
        if(possible_object_indices.size() > 0)
            return *possible_object_indices.end(); //just return the last one, since that should be the top-most
        else
            return -1;
    }

    std::vector<object_index_t> hex_map_t::object_indices_at(glm::vec2 const& world_pos) const
    {
        std::vector<object_index_t> object_inds;

        //grab every object that intersects the position
        size_t obj_index = 0;
        for(auto const& obj : objects)
        {
            auto ub = obj.position + obj.size_d2;
            auto lb = obj.position - obj.size_d2;

            if(world_pos.x >= lb.x && world_pos.x <= ub.x &&
               world_pos.y >= lb.y && world_pos.y <= ub.y)
            {
                object_inds.push_back(obj_index);
            }

            ++obj_index;
        }

        return object_inds;
    }

    ///OPTIMIZE: iterate from end to start and grab the first one that intersects
    spline_index_t hex_map_t::spline_index_at(glm::vec2 const& world_pos) const
    {
        auto possible_spline_indices = spline_indices_at(world_pos);

        if(possible_spline_indices.size() > 0)
            return *possible_spline_indices.end(); //just return the last one, since that should be the top-most
        else
            return -1;
    }

    std::vector<spline_index_t> hex_map_t::spline_indices_at(glm::vec2 const& world_pos) const
    {
        std::vector<spline_index_t> spline_inds;

        size_t spline_ind = 0;
        for(auto const& spline : splines)
        {
            if(point_intersects_spline(world_pos, spline))
                spline_inds.push_back(spline_ind);

            ++spline_ind;
        }

        return spline_inds;
    }

}
}
}