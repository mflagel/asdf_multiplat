#include "stdafx.h"
#include "shader.h"

#include <glm/gtc/type_ptr.hpp> 

#include "utilities.h"
#include "data/gl_state.h"
#include "asdf_multiplat.h"

using namespace asdf::util;

namespace asdf {

    shader_t::shader_t(std::string const& _name, std::string _vshader_filepath, std::string _fshader_filepath)
    : shader_t(_name, _vshader_filepath.c_str(), _fshader_filepath.c_str())
    {
    }

    shader_t::shader_t(std::string const& _name, const char* _vshader_filepath, const char* _fshader_filepath)
        : name(_name)
        , vertex_shader_id(load_shader(_vshader_filepath, GL_VERTEX_SHADER))
        , fragment_shader_id(load_shader(_fshader_filepath, GL_FRAGMENT_SHADER))
        , shader_program_id(UINT32_MAX)
    {
        create_shader_program(vertex_shader_id, fragment_shader_id);
        load_uniforms_and_attributes();
        ASSERT(!CheckGLError(), "Error creating shader \'%s\'", name.c_str());
    }

    shader_t::~shader_t() {
        glDeleteProgram(shader_program_id);
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
    }

    void shader_t::load_uniforms_and_attributes() {
        int total = -1;
        char name_buffer[100];
        int name_len = -1, num = -1;
        GLenum type = GL_ZERO;


        glGetProgramiv(shader_program_id, GL_ACTIVE_UNIFORMS, &total);

        for (int i = 0; i<total; ++i)
        {
            glGetActiveUniform(shader_program_id, GLuint(i), sizeof(name_buffer)-1
                , &name_len, &num, &type, name_buffer);

            name_buffer[name_len] = 0;
            GLint location = glGetUniformLocation(shader_program_id, name_buffer);

            uniforms[name_buffer] = location;
        }

        total = name_len = num = -1;
        glGetProgramiv(shader_program_id, GL_ACTIVE_ATTRIBUTES, &total);

        for (int i = 0; i < total; i++)
        {
            glGetActiveAttrib(shader_program_id, GLuint(i), sizeof(name_buffer)-1
                , &name_len, &num, &type, name_buffer);

            name_buffer[name_len] = 0;
            GLint location = glGetAttribLocation(shader_program_id, name_buffer);

            attributes[name_buffer] = location;
        }
    }

    void shader_t::update_wvp_uniform()
    {
        ASSERT(GL_State->current_shader = shader_program_id, "Shader must be bound before setting uniform");
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
        ASSERT(GL_State->initialized, "Loading a shader before openGL has been initialized");
        ASSERT(!CheckGLError(), "GL Error before loading shader \'%s\'", filepath);

        std::string shader_str = read_text_file(filepath);
        const char* shader_src = shader_str.c_str();

        GLuint shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, static_cast<const GLchar**>(&shader_src), nullptr);
        glCompileShader(shader);

        bool shader_error = CheckGLError(shader);
        if (shader_error) {
            LOG("--------Error in shader source:\n");
            LOG("%s", shader_src);

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
        GLuint shader_program = glCreateProgram();
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
            maxLength = std::max(0, maxLength);
            char* log = new char[size_t(maxLength)];
            glGetProgramInfoLog(shader_program, maxLength, &length, log);

            LOG("--- GL_INFO_LOG ---");
            LOG("%s", log);
            LOG("-------------------");

            delete[] log;
            return GLuint(UINT32_MAX);
        }

        return shader_program;
    }
}