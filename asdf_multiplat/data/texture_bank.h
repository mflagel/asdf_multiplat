#pragma once

#include <string>
#include <vector>
#include <experimental/filesystem>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "gl_resources.h"
#include "utilities/cjson_utils.hpp"

namespace asdf {
namespace data
{
    //TODO: move or rename to something less generic?
    constexpr size_t saved_texture_dim = 128;
    constexpr size_t saved_texture_dim_d2 = saved_texture_dim / 2;
    constexpr size_t max_saved_textures_1d = 10;
    constexpr size_t max_saved_textures = max_saved_textures_1d * max_saved_textures_1d;
    constexpr size_t atlas_dim = saved_texture_dim * max_saved_textures_1d;


    struct saved_texture_t
    {
        std::string name;
        std::experimental::filesystem::path filepath; //filepath as saved in json, which is relative to the json file
        std::experimental::filesystem::path local_filepath;    //canonical filepath 

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };


    /*
        class to load a bunch of textures from the filesystem, resize them,
        and store them in an atlas

        Loads the textures and then renders them directly onto the atlas
        texture at the desired size

        Starts at the bottom left, adding horizontally until the row is
        full, then moving up a row (and so on)

        Thus 0,0 is the bottom left
    */
    struct texture_bank_t
    {
        std::string name;
        texture_t atlas_texture;
        framebuffer_t atlas_fbo;

        std::vector<saved_texture_t> saved_textures;

        texture_bank_t(std::string name = "unnamed texture bank");

        void load_from_file(std::experimental::filesystem::path const& filepath);
        void save_to_file(std::experimental::filesystem::path const& filepath);

        cJSON* to_JSON() const;
        std::string save_to_string() const;
        std::string save_to_string_unformatted() const;

        void add_texture(saved_texture_t const&);
        void add_textures(std::vector<saved_texture_t> const&);

        void add_texture(std::experimental::filesystem::path const& texture_path);
        void add_textures(std::vector<std::experimental::filesystem::path> const& filepaths, std::experimental::filesystem::path const& relative_dir = std::experimental::filesystem::path());
        void add_textures_from_asset_dir(std::vector<std::experimental::filesystem::path> const& filepaths);

        void clear();
    };
}
}