#pragma once

#include <exception>
#include <string>

#include <glm/glm.hpp>
#include <SOIL/SOIL.h>

using color_t = glm::vec4;

namespace asdf {

    struct content_load_exception : std::exception 
    {
        std::string filepath;
        std::string loadError;
        content_load_exception(std::string const& filepath, std::string const& loadError);
        virtual const char* what() const noexcept;
    };

    struct texture_t 
    {
        std::string name = "";
        GLuint texture_id   = UINT32_MAX;
        uint32_t width      = UINT32_MAX;
        uint32_t height     = UINT32_MAX;
        uint32_t halfwidth  = UINT32_MAX;
        uint32_t halfheight = UINT32_MAX;
        GLenum format       = UINT32_MAX;
        //int numChannels = 0;
        bool is_compressed = false;
        GLint types[5];

        inline GLuint     get_textureID() const { return texture_id; }
        inline uint32_t       get_width()     const { return width;      }
        inline uint32_t       get_height()    const { return height;     }
        inline glm::uvec2 get_size()      const { return glm::uvec2(width, height); }

        texture_t();

        texture_t(std::string const& filepath, int force_channels = SOIL_LOAD_AUTO);
        texture_t(std::string const& name, std::string const& filepath);
        texture_t(std::string const& name, const GLuint texture_index);
        //FIXME: replace {ptr, len} with array_view_<T>
        texture_t(std::string const& _name, color_t* color_data, uint32_t _width, uint32_t _height, bool generate_mipmaps = false);

        ~texture_t();

        void refresh_params();
        void load_texture(std::string const& filepath, int force_channels = SOIL_LOAD_AUTO);
        
        void write(const color_t* color_data);
        void write(const color_t* color_data, const uint32_t _width, const uint32_t _height);

        glm::ivec2 texture_to_screen_space(glm::ivec2 const& texture_pos) const;
        glm::ivec2 screen_to_texture_space(glm::ivec2 const& screen_pos) const;
    };

    //in texture space, {0,0} is bottom left, {n-1,n-1} is top right
    glm::ivec2 texture_to_screen_space(glm::ivec2 const& texture_pos, glm::ivec2 const& texture_halfsize);
    glm::ivec2 screen_to_texture_space(glm::ivec2 const& screen_pos, glm::ivec2 const& texture_halfsize);

    
}
