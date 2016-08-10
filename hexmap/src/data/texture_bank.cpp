#include "stdafx.h"
#include "texture_bank.h"

#include <glm/gtc/matrix_transform.hpp>

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
            GL_State->bind(atlas_fbo);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, atlas_texture.texture_id, 0);

            GLenum draw_buffers = GL_COLOR_ATTACHMENT1;
            glDrawBuffers(1, &draw_buffers);
            ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "GL Error creating texture bank framebuffer");

            glViewport(0,0,atlas_texture.width, atlas_texture.height);

            glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            GL_State->unbind_fbo();
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
        auto prev_fbo = GL_State->current_framebuffer;

        ASSERT(is_file(filepath), "File not found %s", filepath.c_str());
        texture_t new_texture(filepath, SOIL_LOAD_RGBA); //force RGBA, since that's what the atlas uses. Might not be neccesary now that I'm rendering to a framebuffer

        //ASSERT(new_texture.format == atlas_texture.format, "Color format of texture must match the atlas (GL_RGBA)   %s", filepath.c_str());
        //ASSERT(new_texture.types[0] == atlas_texture.types[0], "");

        int dest_loc_x = (saved_textures.size() % max_saved_textures_1d) * saved_texture_dim;
        int dest_loc_y = (saved_textures.size() / max_saved_textures_1d) * saved_texture_dim;


        GL_State->bind(atlas_fbo);
        glViewport(0,0,atlas_texture.width, atlas_texture.height);

        ASSERT(!CheckGLError(), "");
        auto& screen_shader = app.renderer->screen_shader;
        GL_State->bind(screen_shader);

        screen_shader->world_matrix[3][0] = dest_loc_x;
        screen_shader->world_matrix[3][1] = dest_loc_y;
        screen_shader->world_matrix = glm::scale(screen_shader->world_matrix, glm::vec3(1000.0f, 1000.0f, 1.0f));
        screen_shader->projection_matrix = glm::ortho<float>(0, atlas_texture.width, 0, atlas_texture.height/*, -1.0f, 1.0f*/);
        screen_shader->update_wvp_uniform();

        glUniform4f(screen_shader->uniform("Color"), 1.5f, 1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, new_texture.texture_id);

        app.renderer->quad.render();
        
        //re-bind prev fbo
        glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
        GL_State->current_framebuffer = prev_fbo;


        saved_textures.push_back(saved_texture_t{filepath});

        ASSERT(!CheckGLError(), "GL Error in texture_bank_t::add_texture() for \'%s\'", filepath.c_str());
    }
}
}
}