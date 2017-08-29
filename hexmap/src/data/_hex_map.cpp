#include "stdafx.h"
#include "hex_map.h"

#include "asdf_multiplat/main/asdf_multiplat.h"
#include "asdf_multiplat/data/gl_resources.h"
#include "asdf_multiplat/data/content_manager.h"


using namespace std;

namespace asdf
{
    using namespace util;
    using namespace data;

namespace hexmap
{
namespace data
{
    constexpr const char* default_map_name = "unnamed map";
    constexpr size_t hxm_version = 1;

    constexpr char terrain_types_json_filename[] = "terrain_types.json";
    

    hex_map_t::hex_map_t(std::string const& _map_name, glm::uvec2 grid_size, hex_grid_cell_t const& default_cell_style)
    : map_name(_map_name)
    , hex_grid(grid_size, default_cell_style)
    , terrain_bank(std::string("hexmap terrain"))
    {
        auto data_dir = find_folder("data");
        
        auto terrain_types_json_filepath = data_dir + "/" + string(terrain_types_json_filename);
        terrain_bank.saved_textures.clear(); //reset so I'm not infinitely piling stuff on
        terrain_bank.load_from_file(terrain_types_json_filepath);

        objects_atlas = make_unique<texture_atlas_t>(string(data_dir + "/../assets/Objects/objects_atlas_data.json"));
    }

    hex_map_t::hex_map_t(glm::uvec2 grid_size)
    : hex_map_t(default_map_name, grid_size)
    {}

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


    size_t hex_map_t::object_index_at(glm::vec2 const& world_pos) const
    {
        auto possible_objects = object_indices_at(world_pos);

        if(possible_objects.size() > 0)
        {
            size_t closest = 0;
            auto v = objects[possible_objects[0]].position - world_pos;
            auto closest_dist_sq = (v.x*v.x) + (v.y*v.y); //don't need actual length since we're just comparing

            for(size_t i = 1; i < possible_objects.size(); ++i)
            {
                v = objects[possible_objects[i]].position - world_pos;
                auto dist_sq = (v.x*v.x) + (v.y*v.y);

                if(dist_sq < closest_dist_sq)
                {
                    closest = i;
                    closest_dist_sq = dist_sq;
                }
            }

            return possible_objects[closest];
        }
        else
        {
            return -1;
        }
    }

    std::vector<size_t> hex_map_t::object_indices_at(glm::vec2 const& world_pos) const
    {
        std::vector<size_t> object_inds;

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

}
}
}