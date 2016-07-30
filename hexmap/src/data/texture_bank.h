#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "asdf_multiplat/data/texture.h"
#include "asdf_multiplat/data/gl_resources.h"

namespace asdf {
namespace hexmap {
namespace data
{

    constexpr size_t saved_texture_dim = 128;
    constexpr size_t max_saved_textures_1d = 10;
    constexpr size_t max_saved_textures = max_saved_textures_1d * max_saved_textures_1d;
    constexpr size_t hex_atlas_dim = saved_texture_dim * max_saved_textures_1d;

    constexpr char imported_textures_json_filename[] = "imported_textures.json";


    struct saved_texture_t
    {
        std::string filesystem_location;
    };


    /*
        class to load a bunch of textures from the filesystem, resize them,
        and store them in an atlas

        Originally was going to use load the image into its own texture and
        then use glCopyImageSubData to transfer it onto the atlas, but there's
        no way to resize the added texture in the process.

        Seems the only way to resize the original image to a desired size
        is to load it into a texture as normal, but then render it onto a
        framebuffer/texture at the desired size.

        If I'm going to do that, I might as well render directly onto the
        atlas
    */
    struct texture_bank_t
    {
        texture_t atlas_texture;
        framebuffer_object_t atlas_fbo;

        std::vector<saved_texture_t> saved_textures;

        texture_bank_t();

        void add_texture(std::string const& filesystem_location);
    };
}
}
}