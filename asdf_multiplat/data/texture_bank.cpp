#include "stdafx.h"
#include "texture_bank.h"

#include <glm/gtc/matrix_transform.hpp>

#include "main/asdf_multiplat.h"
#include "data/content_manager.h"
#include "data/gl_state.h"
#include "utilities/utilities.h"
#include "utilities/cjson_utils.hpp"

using namespace std;
using namespace glm;

namespace asdf
{
    using namespace util;

namespace data
{

    texture_bank_t::texture_bank_t(string _name)
    : name(_name)
    , atlas_texture(_name + string(" atlas"), nullptr, atlas_dim, atlas_dim)
    {
        glBindTexture(GL_TEXTURE_2D, atlas_texture.texture_id);

        GL_State->init_render_target(atlas_fbo, atlas_texture);

        ASSERT(!CheckGLError(), "GL Error Initializing texture_bank_t");
    }

    void texture_bank_t::load_from_list_file(std::string const& filepath)
    {
        std::string json_str = read_text_file(filepath);
        cJSON* root = cJSON_Parse(json_str.c_str());
        ASSERT(root, "Error loading imported textures json file");

        vector<const char*> textures;
        CJSON_GET_STR_VECTOR(textures);

        add_textures(textures);
        
        cJSON_Delete(root);
    }

    void texture_bank_t::add_texture(std::string const& filepath)
    {
        ASSERT(is_file(filepath), "File not found %s", filepath.c_str());
        texture_t new_texture(filepath, SOIL_LOAD_RGBA); //force RGBA, since that's what the atlas uses. Might not be neccesary now that I'm rendering to a framebuffer

        //ASSERT(new_texture.format == atlas_texture.format, "Color format of texture must match the atlas (GL_RGBA)   %s", filepath.c_str());
        //ASSERT(new_texture.types[0] == atlas_texture.types[0], "");

        int dest_loc_x = (saved_textures.size() % max_saved_textures_1d) * saved_texture_dim;
        int dest_loc_y = (saved_textures.size() / max_saved_textures_1d) * saved_texture_dim;

        dest_loc_x += saved_texture_dim_d2;
        //dest_loc_y += saved_texture_dim_d2;

        scoped_fbo_t scoped(atlas_fbo, 0,0,atlas_texture.width, atlas_texture.height);

        ASSERT(!CheckGLError(), "");
        auto& screen_shader = app.renderer->screen_shader;
        GL_State->bind(screen_shader);

        screen_shader->world_matrix = mat4();
        screen_shader->world_matrix = translate(screen_shader->world_matrix, vec3(static_cast<float>(dest_loc_x), static_cast<float>(dest_loc_y + 64), 0.0f));
        screen_shader->world_matrix = scale(screen_shader->world_matrix, vec3(saved_texture_dim, saved_texture_dim, 1.0f));

        screen_shader->projection_matrix = glm::ortho<float>(0, atlas_texture.width, 0, atlas_texture.height/*, -1.0f, 1.0f*/);
        screen_shader->update_wvp_uniform();

        glUniform4f(screen_shader->uniform("Color"), 1.0f, 1.0f, 1.0f, 1.0f);

        glBindTexture(GL_TEXTURE_2D, new_texture.texture_id);

        // app.renderer->quad.render();
        app.renderer->quad.render_without_vao(screen_shader);


        saved_textures.push_back(saved_texture_t{filepath});
        LOG("Added texture '%s' to '%s'", filepath.c_str(), atlas_texture.name.c_str());

        ASSERT(!CheckGLError(), "GL Error in texture_bank_t::add_texture() for \'%s\'", filepath.c_str());
    }

    /// FIXME: somehow indicate that this takes a list of local filepaths?
    ///        currently this prepends Content.asset_path to the tex_filepath
    void texture_bank_t::add_textures(std::vector<const char*> const& filepaths)
    {
        for(auto const& tex_filepath : filepaths)
        {
            string asset_tex_path = Content.asset_path + "/" + tex_filepath;
            if(is_file(asset_tex_path))
            {
                add_texture(asset_tex_path);
            }
            else
            {
                LOG("Texture not found: %s", tex_filepath);
            }
        }
    }

}
}
