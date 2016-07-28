#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "asdf_multiplat/data/texture.h"

namespace asdf {
namespace hexmap {
namespace data
{

    constexpr size_t saved_texture_dim = 128;
    constexpr size_t max_saved_textures_1d = 10;
    constexpr size_t max_saved_textures = max_saved_textures_1d * max_saved_textures_1d;
    constexpr size_t hex_atlas_dim = saved_texture_dim * max_saved_textures_1d;

    constexpr char imported_textures_json_file[] = "imported_textures.json";


    struct saved_texture_t
    {
        std::string filesystem_location;
    };


    /*
        class to load a bunch of textures from the filesystem, resize them,
        and store them in an atlas

        may use GL_TEXTURE_2D_ARRAY
            Core in version 4.5
            Core since version  3.0
            EXT extension   EXT_texture_array

            Might not exist in mobile, and will have to manually
            create an atlas

        use glCopyImageSubData to copy data from one texture to another
            ex: load new texture, copy+resize into bank texture

    */
    struct texture_bank_t
    {
        texture_t atlas_texture;

        std::vector<saved_texture_t> saved_textures;

        texture_bank_t();

        void add_texture(std::string const& filesystem_location);
    };
}
}
}