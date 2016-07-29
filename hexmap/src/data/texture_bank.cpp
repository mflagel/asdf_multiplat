#include "stdafx.h"
#include "texture_bank.h"

#include "asdf_multiplat/data/gl_resources.h"
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

        {
            GL_State.bind(atlas_fbo);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, atlas_texture.texture_id, 0);
            GLenum draw_buffers = GL_COLOR_ATTACHMENT0;
            glDrawBuffers(1, &draw_buffers);

            ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "GL Error creating texture bank framebuffer");
            GL_State.unbind_fbo();
        }

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
        texture_t new_texture(filepath, SOIL_LOAD_RGBA); //force RGBA, since that's what the atlas uses. Might not be neccesary now that I'm rendering to a framebuffer

        //texture_t new_texture("test", nullptr, 128, 128);

        //ASSERT(new_texture.format == atlas_texture.format, "Color format of texture must match the atlas (GL_RGBA)   %s", filepath.c_str());
        //ASSERT(new_texture.types[0] == atlas_texture.types[0], "");

        int dest_loc_x = (saved_textures.size() % max_saved_textures_1d) * saved_texture_dim;
        int dest_loc_y = (saved_textures.size() / max_saved_textures_1d) * saved_texture_dim;

        //glBindTexture(GL_TEXTURE_2D, 0);

        GL_State.bind(atlas_fbo);
        //glViewport(0,0,atlas_texture.width, atlas_tetxure.height);

        saved_textures.push_back(saved_texture_t{filepath});

        ASSERT(!CheckGLError(), "GL Error in texture_bank_t::add_texture() for \'%s\'", filepath.c_str());
    }
}
}
}