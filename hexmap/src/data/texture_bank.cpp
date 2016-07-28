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

    }

    void texture_bank_t::add_texture(std::string const& filepath)
    {
        texture_t new_texture("", filepath);

        int dest_loc_x = (saved_textures.size() % max_saved_textures_1d) * saved_texture_dim;
        int dest_loc_y = (saved_textures.size() / max_saved_textures_1d) * saved_texture_dim;

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
                          , 0                         // GLsizei srcDepth  might need to be 1?
                         );

        saved_textures.push_back(saved_texture_t{filepath});
    }
}
}
}