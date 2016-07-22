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

    class texture_t 
    {
    public:
        std::string name = "";
        GLuint texture_id = 9001;
        size_t width = 0;
        size_t height = 0;
        size_t halfwidth = 0;
        size_t halfheight = 0;
        int format = 0;
        //int numChannels = 0;

    public:
        inline GLuint    get_textureID() const { return texture_id; }
        inline size_t    get_width()     const { return width;      }
        inline size_t    get_height()    const { return height;     }
        inline glm::vec2 get_size()      const { return glm::vec2(width, height); }

    public:
        texture_t();

        texture_t(std::string const& filepath);
        texture_t(std::string const& name, std::string const& filepath);
        texture_t(std::string const& name, const GLuint texture_index);
        //FIXME: replace {ptr, len} with array_view_<T>
        texture_t(std::string const& _name, color_t* color_data, size_t _width, size_t _height, bool generate_mipmaps = false);

        ~texture_t();

        void refresh_params();
        void load_texture(std::string const& filepath);
        
        void write(const color_t* color_data);
        void write(const color_t* color_data, const size_t _width, const size_t _height);

        glm::ivec2 texture_to_screen_space(glm::ivec2 const& texture_pos) const;
        glm::ivec2 screen_to_texture_space(glm::ivec2 const& screen_pos) const;
    };

    //in texture space, {0,0} is bottom left, {n-1,n-1} is top right
    glm::ivec2 texture_to_screen_space(glm::ivec2 const& texture_pos, glm::ivec2 const& texture_halfsize);
    glm::ivec2 screen_to_texture_space(glm::ivec2 const& screen_pos, glm::ivec2 const& texture_halfsize);

    
}
