#include "stdafx.h"
#include "gl_resources.h"

namespace asdf
{
    gl_state_t GL_State;


    using namespace util;

    void gl_state_t::init_openGL()
    {
        GLenum err = glewInit();
        ASSERT(err == GLEW_OK, "Error: %s\n", glewGetErrorString(err));
        ASSERT(!CheckGLError(), "OpenGL Error after glewInit");

        LOG("### Using OpenGL Version: %s ###\n\n", glGetString(GL_VERSION));


        initialized = true;
        LOG("OpenGL Initialized");
        LOG("--Supported OpenGL Extensions--")

        gl_extensions = tokenize((char*)(glGetString(GL_EXTENSIONS)), " ");

        for(auto const& ext : gl_extensions)
        {
            LOG("%s", ext.c_str());
        }
        LOG("--");

        GLint max_uniform_components = 0;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max_uniform_components);
        LOG("Max Uniform Components: %i", max_uniform_components);

        LOG("--");
    }


    void gl_state_t::bind(vao_t const& vao)
    {
        LOG_IF(current_vao == vao.id, "vao %i already in use", vao);
        glBindVertexArray(vao.id);
        current_vao = vao.id;
    }

    void gl_state_t::bind(vbo_t const& vbo)
    {
        LOG_IF(current_vbo == vbo.id, "vbo %i already in use", vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
        current_vbo = vbo.id;
    }

    void gl_state_t::bind(std::shared_ptr<shader_t> const& shader)
    {
        //LOG_IF(current_shader == shader->shader_program_id, "shader \'%s\' already in use", shader->name.c_str());
        glUseProgram(shader->shader_program_id);
        current_shader = shader->shader_program_id;
    }

    void gl_state_t::unbind_vao()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        current_vao = 0;
    }

    void gl_state_t::unbind_vbo()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        current_vbo = 0;
    }

    void gl_state_t::unbind_shader()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        current_vbo = 0;
    }


    bool gl_state_t::assert_sync()
    {
        //todo:
        //check vbo
        //check vao
        //check shader

        return true;
    }


    /// I'm not entirely sure why these functions return true if there's an error, but whatever

        bool CheckShader(GLuint shader)
        {
            GLint glStatus;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &glStatus);

            if(glStatus == GL_FALSE)
            {
                GLint error_length = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_length);

                std::string gl_error_str(error_length, '\n');
                glGetShaderInfoLog(shader, error_length, nullptr, &(gl_error_str[0]));

                fprintf(stderr, "Failed to compile shader!\n%s\n", gl_error_str.data());
                return true;
            }

            return false;
        }

        bool CheckGLError(GLuint shader)
        {
            bool had_error = false;

            if(shader != nullindex)
            {
                had_error = CheckShader(shader);
            }

            GLenum errCode = GL_NO_ERROR;
            
            while((errCode = glGetError()) != GL_NO_ERROR)
            {
                const GLubyte *errString;
                errString = gluErrorString(errCode);
                fprintf(stderr, "OpenGL Error: %s\n", errString);
                had_error = true;
            }

            return had_error;
        }

        bool CheckGLError(){ return CheckGLError(0xFFFFFFFF); }
}