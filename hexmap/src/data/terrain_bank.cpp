#include "stdafx.h"
#include "terrain_bank.h"

#include "asdf_multiplat/data/content_manager.h"
#include "asdf_multiplat/data/gl_state.h"

using namespace std;
using namespace std::experimental::filesystem;

namespace asdf
{
    using namespace util;
    using namespace data;

namespace hexmap {
namespace data
{

    void terrain_bank_t::load_from_file(path const& filepath)
    {
        texture_bank_t::load_from_file(filepath);

        //rebuild_colors();
    }


    /*
    To programatically get the list of colors for each
    terrain entry, render the atlas in such a way that each
    atlas entry is rendered into one pixel of the colors_texture
    (this is basically the equivilent of generating 1x1 mipmaps
    for each texture, but doing it for all texture entries at once)

    grab colors using glReadPixel() which apparently grabs pixel data
    from a framebuffer (not a texture)
        Do I even need the texture?
        Does it grab from the currently bound fbo?
    */
    // void terrain_bank_t::rebuild_colors()
    // {
    //     GL_State->bind(colors_fbo);

    //     //TODO: render textures as 1x1 squares


    //     glReadBuffer(GL_COLOR_ATTACHMENT0);

    //     int width  = atlas_texture.width  / saved_texture_dim;
    //     int height = atlas_texture.height / saved_texture_dim;
    //     ASSERT((width * height) == max_saved_textures, "");

    //     std::vector<float> buffer(4 * width * height);
    //     //float buffer[4 * width * height]; //4 floats per 
    //     glReadPixels(0,0
    //                , width, height
    //                , GL_RGBA
    //                , GL_FLOAT
    //                , buffer.data());

    //     ASSERT(!CheckGLError(), "GL Error reading pixels from color fbo");
    // }
}
}
}