#include "stdafx.h"
#include "shader.h"

#include <glm/gtc/type_ptr.hpp> 

#include "utilities.h"
#include "data/gl_resources.h"
#include "asdf_multiplat.h"

using namespace asdf::util;

namespace asdf {

    shader_t::shader_t(std::string const& name, std::string vshader_filepath, std::string fshader_filepath)
    : shader_t(name, vshader_filepath.c_str(), fshader_filepath.c_str())
    {
    }

    shader_t::shader_t(std::string const& name, const char* vshader_filepath, const char* fshader_filepath)
        : name(name)
        , vertex_shader_id(load_shader(vshader_filepath, GL_VERTEX_SHADER))
        , fragment_shader_id(load_shader(fshader_filepath, GL_FRAGMENT_SHADER))
        , shader_program_id(create_shader_program(vertex_shader_id, fragment_shader_id))
    {
        load_uniforms();
        ASSERT(!CheckGLError(), "Error creating shader \'%s\'", name.c_str());
    }

    shader_t::~shader_t() {
        glDeleteProgram(shader_program_id);
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
    }

    void shader_t::load_uniforms() {
        int total = -1;
        glGetProgramiv(shader_program_id, GL_ACTIVE_UNIFORMS, &total);
        for (int i = 0; i<total; ++i) {
            int name_len = -1, num = -1;
            GLenum type = GL_ZERO;
            char uniformName[100];
            glGetActiveUniform(shader_program_id, GLuint(i), sizeof(uniformName)-1,
                               &name_len, &num, &type, uniformName);
            uniformName[name_len] = 0;
            GLuint location = glGetUniformLocation(shader_program_id, uniformName);

            uniforms[uniformName] = location;
        }
    }

    void shader_t::update_wvp_uniform()
    {
        ASSERT(!CheckGLError(), "Error before updating shader WVP uniform");
        glm::mat4 wvp = projection_matrix * view_matrix * world_matrix;
        glUniformMatrix4fv(uniforms.at("WVP"), 1, GL_FALSE, glm::value_ptr(wvp));

        ASSERT(!CheckGLError(), "Error updating shader WVP uniform");
    }

    GLint shader_t::uniform(std::string const& uniform_name) const
    {
        return uniforms.at(uniform_name);
    }

    /*static*/GLuint shader_t::load_shader(const char* filepath, GLuint shader_type) 
    {
        LOG("loading shader: %s", filepath);
        ASSERT(util::is_file(filepath), "Shader file \"%s\" does not exist", filepath);
        ASSERT(GL_State.initialized, "Loading a shader before openGL has been initialized");
        ASSERT(!CheckGLError(), "GL Error before loading shader \'%s\'", filepath);

        std::string shader_str = read_text_file(filepath);
        const char* shader_src = shader_str.c_str();

        GLuint shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, static_cast<const GLchar**>(&shader_src), nullptr);
        glCompileShader(shader);

        bool shader_error = CheckGLError(shader);
        if (shader_error) {
            LOG("--------Error in shader source:\n");
            LOG(shader_src);

#ifdef _DEBUG
            int asdfa;
            std::cin >> asdfa;
#endif
            EXPLODE("Error creating shader");
            return GLuint(nullindex);
        }
        
        //free(shader_src);
        return shader;
    }

    /*static*/GLuint shader_t::create_shader_program(const GLuint vs, const GLuint fs) 
    {
        //create
        int shader_program = glCreateProgram();
        ASSERT(CheckGLError() == 0, "Error creating shader program");

        //attach
        glAttachShader(shader_program, vs);
        glAttachShader(shader_program, fs);
        ASSERT(CheckGLError() == 0, "Error attaching vs %i or fs %i", vs, fs);

        //link
        glLinkProgram(shader_program);
        ASSERT(CheckGLError() == 0, "Error linking shader program %i", shader_program);

        //dump any errors
        int spTest;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &spTest);
        if (spTest == GL_FALSE) {
            LOG("Failed to link shader program with vs: %i and fs: %i\n", vs, fs);

            int maxLength = 0;
            int length = 0;
            glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &maxLength);
            char* log = new char[maxLength];
            glGetProgramInfoLog(shader_program, maxLength, &length, log);

            LOG("--- GL_INFO_LOG ---");
            LOG("%s", log);
            LOG("-------------------");

            return GLuint(nullindex);
        }

        return shader_program;
    }

}