#include "stdafx.h"
#include "texture.h"

#include "main/asdf_multiplat.h"
#include "data/gl_state.h"

using namespace glm;

namespace asdf
{
    texture_t::texture_t()
    {
        ASSERT(GL_State->initialized, "Error: Creating texture before openGL has been initialized");
        LOG_IF(CheckGLError(), "GL Error before instantiating blank texture");

        
        glGenTextures(1, &texture_id);

        if(CheckGLError() == 0)
        {
            LOG("Created blank texture with id: %u", texture_id);
            ASSERT(texture_id != 9001, "texture_id not initialized");
        }
        else
        {
            LOG("GL Error creating blank texture. id: %u", texture_id);
        }
    }

    texture_t::texture_t(std::string const& filepath, int force_channels)
    {
        load_texture(filepath, force_channels);
    }
    texture_t::texture_t(std::string const& _name, std::string const& filepath)
    : name(_name)
    {
        load_texture(filepath);
    }
    texture_t::texture_t(std::string const& _name, const GLuint texture_index)
    : name(_name)
    , texture_id(texture_index)
    {
        refresh_params();
    }


    texture_t::texture_t(std::string const& _name, color_t* color_data, size_t _width, size_t _height, bool generate_mipmaps)
    : name{_name}
    , width{_width}
    , height{_height}
    , halfwidth{_width / 2}
    , halfheight{_height / 2}
    , format{GL_RGBA}
    {
        ASSERT(std::div(long(_width), 2L).rem == 0, "Width of texture \'%s\' must be divisible by 2", _name.c_str());
        ASSERT(std::div(long(_height), 2L).rem == 0, "Height of texture \'%s\' must be divisible by 2", _name.c_str());

        glGenTextures(1, &texture_id);
        
        write(color_data);

        //TODO: store this information?
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // REQUIRED to be considered a 'complete' texture. Without this, glCopyImageSubData will fail
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        refresh_params();

        if(generate_mipmaps)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
            ASSERT(!CheckGLError(), "Error loading mipmaps for texture \"%s\" after loading from color_t array", name.c_str());
        }

        LOG_IF(!CheckGLError(), "successfully created texture \"%s\" size {%zu, %zu} from color_t array", name.c_str(), width, height);
    }

    texture_t::~texture_t()
    {
        glDeleteTextures(1, &texture_id);
    }

    void texture_t::write(const color_t* color_data, const size_t _width, const size_t _height)
    {
        width = _width;
        height = _height;
        halfwidth = _width / 2.0f;
        halfheight = _height / 2.0f;

        write(color_data);
        refresh_params();
    }

    void texture_t::write(const color_t* color_data)
    {
        ASSERT(!CheckGLError(), "Error before writing to texture \"%s\" from color_t array", name.c_str());
        ASSERT(texture_id > 0, "");

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D
                   , 0         // mipmap level (0 for base)
                   , GL_RGBA   // format of resulting texture data
                   , width
                   , height
                   , 0         // afaik this is deprecated. openGL docs for 3.3 say "This value must be 0"
                   , GL_RGBA   // format of data in ram. color_t is RGBA
                   , GL_FLOAT
                   , color_data);

        ASSERT(!CheckGLError(), "Error writing to texture \"%s\" from color_t array", name.c_str());
    }

    void texture_t::load_texture(std::string const& filepath, int force_channels)
    {
        texture_id = SOIL_load_OGL_texture(filepath.c_str(),
                                          force_channels, 
                                          SOIL_CREATE_NEW_ID, 
                                          /*SOIL_FLAG_MIPMAPS | */SOIL_FLAG_INVERT_Y);
        if (texture_id == 0)
        {
            auto error = SOIL_last_result();
            throw content_load_exception(filepath, error);
        }

        refresh_params();
    }

    void texture_t::refresh_params()
    {
        int _width, _height;
        GLint _compressed;

        glBindTexture(GL_TEXTURE_2D, texture_id);

            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_height);

            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &_compressed);

            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_TYPE,   &types[0]);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_TYPE, &types[1]);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_TYPE,  &types[2]);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_TYPE, &types[3]);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH_TYPE, &types[4]);
        
        glBindTexture(GL_TEXTURE_2D, 0);

        width = glm::abs(_width);
        height = glm::abs(_height);
        is_compressed = _compressed == 1;
    }

    ivec2 texture_t::texture_to_screen_space(ivec2 const& texture_pos) const
    {
        return ::asdf::texture_to_screen_space(texture_pos, ivec2{halfwidth, halfheight});
    }
    ivec2 texture_t::screen_to_texture_space(ivec2 const& screen_pos) const
    {
        return ::asdf::screen_to_texture_space(screen_pos, ivec2{halfwidth, halfheight});
    }

    //in texture space, {0,0} is bottom left, {n-1,n-1} is top right
    ivec2 texture_to_screen_space(ivec2 const& texture_pos, ivec2 const& texture_halfsize)
    {
        return ivec2(texture_pos.x - texture_halfsize.x, texture_pos.y - texture_halfsize.y);
    }
    ivec2 screen_to_texture_space(ivec2 const& screen_pos, ivec2 const& texture_halfsize)
    {
        return ivec2(screen_pos.x + texture_halfsize.x, screen_pos.y + texture_halfsize.y);
    }
}