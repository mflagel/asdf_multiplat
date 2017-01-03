#pragma once

#include <unordered_map>

namespace asdf {

    struct shader_t
    {
        std::string name;
        GLuint vertex_shader_id;
        GLuint fragment_shader_id;
        GLuint shader_program_id;

        std::unordered_map<std::string, GLint> uniforms;
        std::unordered_map<std::string, GLint> attributes;

        glm::mat4 world_matrix;
        glm::mat4 view_matrix;
        glm::mat4 projection_matrix;

        shader_t(std::string const& name, std::string vshader_filepath, std::string fshader_filepath);
        shader_t(std::string const& name, const char* vshader_filepath, const char* fshader_filepath);
        ~shader_t();

        shader_t(const shader_t&) = delete;
        shader_t& operator=(const shader_t& rhs) = delete;

        inline void use_program() { glUseProgram(shader_program_id); }  //TODO: deprecate in favour of GL_State::bind_shader
        void load_uniforms_and_attributes();
        void update_wvp_uniform();

        GLint uniform(std::string const& uniform_name) const;

        static GLuint load_shader(const char* filepath, GLuint shader_type);
        static GLuint create_shader_program(const GLuint vs, const GLuint fs);
    };
}