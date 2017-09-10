#include "stdafx.h"
#include "asdf_defs.h"
#include "texture_bank.h"

#include <glm/gtc/matrix_transform.hpp>

#include "main/asdf_multiplat.h"
#include "data/content_manager.h"
#include "data/gl_state.h"
#include "utilities/utilities.h"
#include "utilities/cjson_utils.hpp"

using namespace std;
using namespace glm;

namespace stdfs = std::experimental::filesystem;
using path = stdfs::path;

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
    
    path hunt_for_file(path const& _p)
    {
        constexpr size_t num_upwards_attempts = 3;

        //if relative path, look upwards a bit
        if(_p.is_relative())
        {
            path p = _p;
            for(size_t i = 0; i < num_upwards_attempts; ++i)
            {
                path up_p = "../";
                up_p += p;
                if(stdfs::exists(p))
                    return p;
            }
        }

        //do a more detailed search
        path start_point = _p.parent_path().parent_path();
        path search = asdf::util::find_file(_p.filename(), start_point);

        if(!search.empty())
            return search;

        return path();
    }

    void texture_bank_t::load_from_file(path const& filepath)
    {
        ASSERT(is_regular_file(filepath), "Not a valid file");

        std::string json_str = read_text_file(filepath.string());
        cJSON* root = cJSON_Parse(json_str.c_str());
        ASSERT(root, "Error loading imported textures json file");

        vector<saved_texture_t> terrain;
        //CJSON_GET_ITEM_VECTOR(terrain);

        ASSERT(root && root->child, "invalid texture bank json;");
        cJSON* terrain_json = root->child;

        name = terrain_json->string;
        for(cJSON* arr_item = terrain_json->child; arr_item; arr_item = arr_item->next)
        {
            terrain.push_back(saved_texture_t());
            terrain.back().from_JSON(arr_item);
        }

        auto parent_path = filepath.parent_path();

        for(auto& t : terrain)
        {
            //filepaths in json are relative to the json document itself
            if(!t.filepath.is_absolute())
                t.local_filepath = parent_path / t.filepath;
            else
                t.local_filepath = t.filepath;

            try{
                t.local_filepath = stdfs::canonical(t.local_filepath);
            }
            catch (stdfs::filesystem_error const& fs_e)
            {
                ///try to hunt around for the file
                auto p = hunt_for_file(t.local_filepath);

                if(stdfs::exists(p))
                    t.local_filepath = p;
                else
                    throw fs_e; //just rethrow
            }
        }

        add_textures(terrain);

        cJSON_Delete(root);
    }

    void texture_bank_t::save_to_file(std::experimental::filesystem::path const& filepath)
    {

        /// FIXME: I dislike that this is a side effect. Should rename this func somehwat?
        for(auto& t : saved_textures)
        {
            ///t.filepath = std::filesystem::relative(t.filepath, filepath);  /// MSVC Y U NO SUPPORT FULL C++17 FILESYSTEM!!
            t.filepath = asdf::util::relative(t.local_filepath, filepath);
        }

        write_text_file(filepath.string(), save_to_string());
    }

    cJSON* texture_bank_t::to_JSON() const
    {
        CJSON_CREATE_ROOT();
        
        cJSON* textures_json = cJSON_CreateArray();
        for(auto const& t : saved_textures)
            cJSON_AddItemToArray(textures_json, t.to_JSON());

        cJSON_AddItemToObject(root, name.c_str(), textures_json);

        return root;
    }

    std::string texture_bank_t::save_to_string() const
    {
        return asdf::json_to_string(to_JSON());
    }

    std::string texture_bank_t::save_to_string_unformatted() const
    {
        return json_to_string_unformatted(to_JSON());
    }

    void texture_bank_t::add_texture(saved_texture_t const& added_texture)
    {
        path const& texture_path = added_texture.local_filepath;
        ASSERT(is_regular_file(texture_path), "File not found %s", texture_path.c_str());

        texture_t new_texture(texture_path.string(), SOIL_LOAD_RGBA); //force RGBA, since that's what the atlas uses. Might not be neccesary now that I'm rendering to a framebuffer

        //ASSERT(new_texture.format == atlas_texture.format, "Color format of texture must match the atlas (GL_RGBA)   %s", filepath.c_str());
        //ASSERT(new_texture.types[0] == atlas_texture.types[0], "");

        int64_t dest_loc_x = (saved_textures.size() % max_saved_textures_1d) * saved_texture_dim;
        int64_t dest_loc_y = (saved_textures.size() / max_saved_textures_1d) * saved_texture_dim;

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


        saved_textures.push_back(added_texture);
        LOG("Added texture '%s' to '%s'", texture_path.c_str(), atlas_texture.name.c_str());

        ASSERT(!CheckGLError(), "GL Error in texture_bank_t::add_texture() for \'%s\'", texture_path.c_str());
    }

    void texture_bank_t::add_textures(std::vector<saved_texture_t> const& added_textures)
    {
        for(auto const& t : added_textures)
            add_texture(t);
    }


    void texture_bank_t::add_texture(path const& texture_path)
    {
        add_texture(saved_texture_t{texture_path.stem().string()
                                  , texture_path
                                  , std::experimental::filesystem::canonical(texture_path)
                                  });
    }

    void texture_bank_t::add_textures(std::vector<path> const& filepaths, path const& relative_dir)
    {
        ASSERT(relative_dir.empty() || is_directory(relative_dir), "relative directory is not a directory at all!");

        for(path p : filepaths)
        {
            if(!relative_dir.empty())
            {
                ASSERT(p.is_relative(), "Texture path is absolute, even though it should be relative to %s", relative_dir.c_str());
                p = relative_dir / p;
            }

            if(is_regular_file(p))
            {
                add_texture(p);
            }
            else
            {
                LOG("Texture not found: %s", p.c_str());
            }
        }
    }

    void texture_bank_t::add_textures_from_asset_dir(std::vector<path> const& filepaths)
    {
        add_textures(filepaths, path(Content.asset_path));
    }

    void texture_bank_t::clear()
    {
        saved_textures.clear();
    }


    cJSON* saved_texture_t::to_JSON() const
    {
        CJSON_CREATE_ROOT();
        CJSON_ADD_STR(name);
        ASSERT(filepath.string().size() > 0, "Unexpected empty filepath");
        cJSON_AddStringToObject(root, "filepath", filepath.string().c_str());

        return root;
    }

    void saved_texture_t::from_JSON(cJSON* root)
    {
        CJSON_GET_STR(name);
        auto path_str = string(cJSON_GetObjectItem(root, "filepath")->valuestring);
        asdf::util::replace(path_str, "\\", "/");
        filepath = std::experimental::filesystem::path(path_str);
    }

}
}
