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

        ASSERT(!CheckGLError(), "GL Error allocating texture");
        ASSERT(texture_id != 9001, "texture_id not initialized");

        /// bind texture and give it some default sampler stuff
        /// setting min_filter is REQUIRED to be considered a 'complete' texture.
        /// Without it, glCopyImageSubData will fail (among other things)
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glBindTexture(GL_TEXTURE_2D, 0);

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


    texture_t::texture_t(std::string const& _name, color_t* color_data, uint32_t _width, uint32_t _height, bool generate_mipmaps)
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

        LOG_IF(!CheckGLError(), "successfully created texture \"%s\" size {%u, %u} from color_t array", name.c_str(), width, height);
    }

    texture_t::~texture_t()
    {
        glDeleteTextures(1, &texture_id);
    }

    void texture_t::write(const color_t* color_data, const uint32_t _width, const uint32_t _height)
    {
        width = _width;
        height = _height;
        halfwidth = _width / 2;
        halfheight = _height / 2;

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
                   , unsigned_to_signed(width)
                   , unsigned_to_signed(width)
                   , 0         // afaik this is deprecated. openGL docs for 3.3 say "This value must be 0"
                   , GL_RGBA   // format of data in ram. color_t is RGBA
                   , GL_FLOAT
                   , color_data);

        ASSERT(!CheckGLError(), "Error writing to texture \"%s\" from color_t array", name.c_str());
    }

    /// Most of this code is based on SOIL_load_OGL_texture()  in SOIL.C 
    /// There was no way to get the texture dimensions without grabbing them from openGL
    /// but if the texture size wasnt a power of two, openGL could give incorrect values
    void texture_t::load_texture(std::string const& filepath, int force_channels)
    {
        int channels = 0;

        int w = 0;
        int h = 0;
        unsigned char* img = SOIL_load_image(filepath.c_str(), &w, &h, &channels, force_channels);

        width  = signed_to_unsigned(w);
        height = signed_to_unsigned(h);

        // SOIL_load_image gives the original number of channels even when we force
        // however the resulting image data has the forced format
        channels = (force_channels > 0) ? force_channels : channels;

        if(img == nullptr)
        {
            auto error = SOIL_last_result();
            throw content_load_exception(filepath, error);
        }

        /// COPYPASTE FROM SOIL.C  SOIL_internal_create_OGL_texture()
        /// Invert Image
		int i, j;
		for( j = 0; j*2 < h; ++j )
		{
			int index1 = j * w * channels;
			int index2 = (h - 1 - j) * w * channels;
			for( i = w * channels; i > 0; --i )
			{
				unsigned char temp = img[index1];
				img[index1] = img[index2];
				img[index2] = temp;
				++index1;
				++index2;
			}
		}


        glGenTextures(1, &texture_id);
        ASSERT(!CheckGLError(), "Error allocating GL texture for %s", filepath.c_str());
        glBindTexture(GL_TEXTURE_2D, texture_id);
        ASSERT(!CheckGLError(), "Error binding GL texture for %s", filepath.c_str());

        switch(channels)
        {
            case 1: format = GL_LUMINANCE; break;
            case 2: format = GL_LUMINANCE_ALPHA; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
            default: EXPLODE("invalid image channel count");
        }


        glTexImage2D(GL_TEXTURE_2D
                   , 0 //level 0
                   , unsigned_to_signed(format)  //format of the resulting opengl texture
                   , unsigned_to_signed(width)
                   , unsigned_to_signed(height)
                   , 0                 //some deprecated value
                   , format            //format of the image data in memory
                   , GL_UNSIGNED_BYTE  //supposedly SOIL loads image data as ubytes
                   , img);

        ASSERT(!CheckGLError(), "GL Error loading SOIL image data into OpenGL Texture for %s", filepath.c_str());

        //ignore mipmapping and clamp uv sampling
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
        ASSERT(!CheckGLError(), "GL Error setting texture filter / wrap paramaters" );
        
        if (texture_id == 0)
        {
            auto error = SOIL_last_result();
            throw content_load_exception(filepath, error);
        }


        SOIL_free_image_data(img);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void texture_t::refresh_params()
    {
        int _width, _height;
        GLint _compressed;

        glBindTexture(GL_TEXTURE_2D, texture_id);

            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_height);

            int iformat;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &iformat);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &_compressed);
            format = signed_to_unsigned(iformat);

            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_TYPE,   &types[0]);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_TYPE, &types[1]);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_TYPE,  &types[2]);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_TYPE, &types[3]);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH_TYPE, &types[4]);
        
        glBindTexture(GL_TEXTURE_2D, 0);

        width = signed_to_unsigned(glm::abs(_width));
        height = signed_to_unsigned(glm::abs(_height));
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