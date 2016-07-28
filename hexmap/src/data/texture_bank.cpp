#include "stdafx.h"
#include "texture_bank.h"

#include "asdf_multiplat/utilities/utilities.h"
#include "asdf_multiplat/utilities/cjson_utils.hpp"

using namespace std;

namespace asdf
{
    using namespace util;

namespace hexmap {
namespace data
{

    texture_bank_t::texture_bank_t()
    : atlas_texture("hex texture atlas", nullptr, hex_atlas_dim, hex_atlas_dim)
    {
        ASSERT(!CheckGLError(), "GL Error Before Initializing texture_bank_t");

        auto dir = find_folder("data");
        ASSERT(dir.length() > 0, "Could not find data folder");

        auto imported_textures_json_filepath = dir + "/" + string(imported_textures_json_filename);

        std::string json_str = read_text_file(imported_textures_json_filepath);
        cJSON* root = cJSON_Parse(json_str.c_str());
        ASSERT(root, "Error loading imported textures json file");

        vector<char*> textures;
        CJSON_GET_STR_VECTOR(textures);

        for(auto const& filepath : textures)
        {
            if(is_file(filepath))
            {
                add_texture(filepath);
            }
            else
            {
                LOG("Texture not found at %s", filepath);
            }
        }
        
        cJSON_Delete(root);

        ASSERT(!CheckGLError(), "GL Error Initializing texture_bank_t");
    }

    void texture_bank_t::add_texture(std::string const& filepath)
    {
        ASSERT(is_file(filepath), "File not found %s", filepath.c_str());
        texture_t new_texture(filepath, SOIL_LOAD_RGBA); //force RGBA, since that's what the atlas uses

        //texture_t new_texture("test", nullptr, 128, 128);

        ASSERT(new_texture.format == atlas_texture.format, "Color format of texture must match the atlas (GL_RGBA)   %s", filepath.c_str());
        ASSERT(new_texture.types[0] == atlas_texture.types[0], "");

        int dest_loc_x = (saved_textures.size() % max_saved_textures_1d) * saved_texture_dim;
        int dest_loc_y = (saved_textures.size() / max_saved_textures_1d) * saved_texture_dim;

        glBindTexture(GL_TEXTURE_2D, 0);

        //TODO: wrap this in a gl_state_t function
        glCopyImageSubData( new_texture.texture_id    // GLuint srcName
                          , GL_TEXTURE_2D             // srcTarget
                          , 0                         // mipmap level
                          , 0                         // srcX
                          , 0                         // srcY
                          , 0                         // srcZ
                          , atlas_texture.texture_id  // GLuint dstName
                          , GL_TEXTURE_2D             // dstTarget
                          , 0                         // mipmap
                          , dest_loc_x                // GLint dstX
                          , dest_loc_y                // GLint dstY
                          , 0                         // GLint dstZ
                          , new_texture.width         // GLsizei srcWidth
                          , new_texture.height        // GLsizei srcHeight
                          , 1                         // GLsizei srcDepth needs to be 1 for a 2D texture apparently
                         );

        /*
        GL_INVALID_OPERATION is generated if the texel size of the uncompressed image is
        not equal to the block size of the compressed image.

        GL_INVALID_OPERATION is generated if either object is a texture and the texture is
        not complete.

        GL_INVALID_OPERATION is generated if the source and destination internal formats are
        not compatible, or if the number of samples do not match.
        */

        saved_textures.push_back(saved_texture_t{filepath});

        ASSERT(!CheckGLError(), "GL Error in texture_bank_t::add_texture() for \'%s\'", filepath.c_str());
    }
}
}
}