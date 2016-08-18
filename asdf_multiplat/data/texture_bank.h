#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "asdf_multiplat/data/texture.h"
#include "asdf_multiplat/data/gl_resources.h"

namespace asdf {
namespace data
{

    constexpr size_t saved_texture_dim = 128;
    constexpr size_t saved_texture_dim_d2 = saved_texture_dim / 2.0f;
    constexpr size_t max_saved_textures_1d = 10;
    constexpr size_t max_saved_textures = max_saved_textures_1d * max_saved_textures_1d;
    constexpr size_t atlas_dim = saved_texture_dim * max_saved_textures_1d;


    struct saved_texture_t
    {
        std::string filesystem_location;
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
        texture_t atlas_texture;
        framebuffer_t atlas_fbo;

        std::vector<saved_texture_t> saved_textures;

        texture_bank_t();

        void load_from_list_file(std::string const& filepath);
        void add_texture(std::string const& filesystem_location);
    };
}
}