#pragma once

#include <unordered_map>

namespace asdf {

    class shader_t {
    public:
        std::string name;
        GLuint vertex_shader_id;
        GLuint fragment_shader_id;
        GLuint shader_program_id;

        std::unordered_map<std::string, GLint> uniforms;

        glm::mat4 world_matrix;
        glm::mat4 view_matrix;
        glm::mat4 projection_matrix;

        shader_t(std::string const& name, const char* vshader_filepath, const char* fshader_filepath);
        ~shader_t();

        shader_t(const shader_t&) = delete;
        shader_t& operator=(const shader_t& rhs) = delete;

        inline void use_program() { glUseProgram(shader_program_id); }  //TODO: deprecate in favour of GL_State::bind_shader
        void load_uniforms();
        void update_wvp_uniform();

        static GLuint load_shader(const char* filepath, GLuint shader_type);
        static GLuint create_shader_program(const GLuint vs, const GLuint fs);


    };

#define MAKE_SHARED_SHADER(shader_name) (make_shared<shader_t>(#shader_name, VSHADER_PATH(#shader_name), FSHADER_PATH(#shader_name)))
#define MAKE_SHARED_SHADER_(shader_name, glsl_vers) (make_shared<shader_t>(#shader_name, VSHADER_PATH(#shader_name, glsl_vers), FSHADER_PATH(#shader_name, glsl_vers)))
}