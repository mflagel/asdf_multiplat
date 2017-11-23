#include "stdafx.h"
#include "hex_map.h"

#include <cstddef>

#include <glm/gtx/norm.hpp>

#include "asdf_multiplat/main/asdf_multiplat.h"
#include "asdf_multiplat/data/gl_resources.h"
#include "asdf_multiplat/data/content_manager.h"



using namespace std;
namespace stdfs = std::experimental::filesystem;

namespace asdf
{
    using namespace util;
    using namespace data;

namespace hexmap
{
namespace data
{
    constexpr const char* default_map_name = "unnamed map";

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


    ///OPTIMIZE: iterate from end to start and grab the first one that intersects
    size_t hex_map_t::object_index_at(glm::vec2 const& world_pos) const
    {
        auto possible_object_indices = object_indices_at(world_pos);
        
        if(possible_object_indices.size() > 0)
            return possible_object_indices.back(); //just return the last one, since that should be the top-most
        else
            return nullindex;
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
            return nullindex;
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

    constexpr const char* compressed_ext   = ".compressed";
    constexpr const char* map_data_ext     = ".map_data";
    constexpr const char* terrain_data_ext = ".terrain.json";

    void hex_map_t::save_to_file(std::string const& filepath)
    {
        using namespace asdf::util;

        std::vector<stdfs::path> map_filepaths;

        stdfs::path map_data_filepath(filepath + map_data_ext);
        _save_to_file(map_data_filepath);
        map_filepaths.emplace_back(std::move(map_data_filepath));

        // path compressed_map_filepath(map_data_filepath + compressed_ext);
        // compress_file(compressed_map_filepath);
        // map_filepaths.emplace_back(compressed_map_filepath);

        /// Save terrain_bank as a separate file
        /// Will get compressed with the rest of the save file at some point
        stdfs::path terrain_path(filepath + terrain_data_ext);
        terrain_bank.save_to_file(terrain_path);
        map_filepaths.emplace_back(std::move(terrain_path));

        package_map(map_filepaths, stdfs::path(filepath));
    }

    void hex_map_t::load_from_file(std::string const& filepath)
    {
        unpackage_map(filepath);

        /// load terrain first so it exists when the map loads
        stdfs::path terrain_path(filepath + terrain_data_ext);
        terrain_bank.clear(); /// TODO: cache existing terrain rather than reloading entirely
        terrain_bank.load_from_file(terrain_path);

        stdfs::path map_filepath = filepath + map_data_ext;
        _load_from_file(map_filepath);

        ///remove once they've been loaded
        ///archived file will still be there
        stdfs::remove(terrain_path);
        stdfs::remove(map_filepath);
    }


    //TODO: refactor save/load code such that hex_grid_t::save_to_file() takes a SDL_RWops*
    //      to read/write to

    // https://wiki.libsdl.org/SDL_RWops?highlight=%28%5CbCategoryStruct%5Cb%29%7C%28CategoryIO%29
    // https://wiki.libsdl.org/SDL_RWwrite?highlight=%28%5CbCategoryIO%5Cb%29%7C%28CategoryEnum%29%7C%28CategoryStruct%29
    void hex_map_t::_save_to_file(stdfs::path const& filepath)
    {
        //SDL_RWops* io = SDL_RWFromFile(filepath.c_str(), "wb");

        //stdfs::path::c_str() returns a wchar_t array on windows. 
        //so I'll be lazy and conver to std::string first
        SDL_RWops* io = SDL_RWFromFile(filepath.string().c_str(), "wb");

        if(!io)
        {
            EXPLODE("data::hex_map_t::save_to_file() error");  //todo: handle errors better
        }

        hxm_header_t header;
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

    void hex_map_t::_load_from_file(stdfs::path const& filepath)
    {
        //SDL_RWops* io = SDL_RWFromFile(filepath.c_str(), "rb");
        SDL_RWops* io = SDL_RWFromFile(filepath.string().c_str(), "rb");

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
            objects.resize(header.num_map_objects);
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

    stdfs::path make_temp_path(stdfs::path const& path)
    {
        auto temp_filepath = path;
        do
        {
            temp_filepath += "__temp";
        }
        while (stdfs::exists(temp_filepath));

        return temp_filepath;
    }

    /// TODO: put temporary compression / archive files in a temp folder?
    ///       ie stdfs::temp_directory_path() / "hexmap" / package_filepath /
    ///       will require copying over terrain json before loading
    ///       or modifying load code to specify a directory that paths
    ///       should be relative to
    void hex_map_t::package_map(std::vector<stdfs::path> const& map_filepaths, stdfs::path const& package_filepath)
    {
        stdfs::path archive_filepath = package_filepath;
        archive_filepath += ".tar";

        int tar_result = archive_files(map_filepaths, archive_filepath);
        if(tar_result != 0)
        {
            fprintf(stderr, "tar_open(): %s\n", strerror(errno));
            EXPLODE("error archiving map");
        }
        

        /// write to a .compressed file rather than writing directly over
        /// a file that might already exist at package_filepath
        stdfs::path compressed_filepath = archive_filepath;
        archive_filepath += ".compressed";

        int r = asdf::util::compress_file(archive_filepath, compressed_filepath);
        if(r != 0)
            EXPLODE("failed to compress file [%s]", compressed_filepath.c_str());
        

        /// move/overwrite package_filepath with compressed package and clean up temp archive
        stdfs::rename(compressed_filepath, package_filepath);
        stdfs::remove(archive_filepath);
    }

    void hex_map_t::unpackage_map(stdfs::path const& filepath)
    {
        stdfs::path decompressed_path = filepath;
        decompressed_path += ".tar";

        int r = asdf::util::decompress_file(filepath, decompressed_path);
        if(r != 0)
        {
            EXPLODE("failed to decompress package: %s", filepath.string().c_str());
        }

        int tar_result = unarchive_files(filepath, filepath.parent_path());
        if(tar_result != 0)
        {
            fprintf(stderr, "tar_open(): %s\n", strerror(errno));
            EXPLODE("error extracting package data from archive");
        }

        stdfs::remove(decompressed_path);
    }

    /*
    void hex_map_t::unpackage_map(stdfs::path const& filepath)
    {
        int tar_result = unarchive_files(filepath, filepath.parent_path());
        if(tar_result != 0)
        {
            fprintf(stderr, "tar_open(): %s\n", strerror(errno));
            EXPLODE("error extracting map from archive");
        }

        {
            stdfs::path uncompressed = filepath.string() + map_data_ext;
            stdfs::path compressed = uncompressed.string() + compressed_ext;
            int r = asdf::util::decompress_file(compressed, uncompressed);
            if(r != 0)
            {
                EXPLODE("failed to decompress map data");
            }
            stdfs::remove(compressed);
        }
        {
            stdfs::path uncompressed = filepath.string() + terrain_data_ext;
            stdfs::path compressed = uncompressed.string() + compressed_ext;
            int r = asdf::util::decompress_file(compressed, uncompressed);
            if(r != 0)
            {
                EXPLODE("failed to decompress terrain json");
            }
            stdfs::remove(compressed);
        }
    }
    */

}
}
}
