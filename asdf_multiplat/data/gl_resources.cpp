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

        //TODO: store an enum and array of relevant gl info?

        GLint max_uniform_components = 0;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max_uniform_components);
        LOG("Max Uniform Components: %i", max_uniform_components);

        GLint max_texture_size = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
        LOG("Max Texture Size: %i", max_texture_size);

        GLint max_texture_units = 0;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_texture_units);
        LOG("Max Texture Units: %i", max_texture_units);

        LOG("--");

        ASSERT(!CheckGLError(), "Error initializing openGL");
    }


    void gl_state_t::bind(vao_t const& vao)
    {
        LOG_IF(current_vao == vao.id, "vao %i already in use", vao.id);
        glBindVertexArray(vao.id);
        current_vao = vao.id;
    }

    void gl_state_t::bind(std::shared_ptr<shader_t> const& shader)
    {
        //LOG_IF(current_shader == shader->shader_program_id, "shader \'%s\' already in use", shader->name.c_str());
        glUseProgram(shader->shader_program_id);
        current_shader = shader->shader_program_id;
    }

    void gl_state_t::bind(framebuffer_object_t const& fbo)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
        current_framebuffer = fbo.id;
    }

    void gl_state_t::bind(gl_buffer_object_t const& buffer)
    {
        LOG_IF(current_buffers[buffer.target] == buffer.id, "buffer  %i already in use for target %s", buffer.id, gl_buffer_target_strings[buffer.target]);

        glBindBuffer(gl_buffer_target_enum_values[buffer.target], buffer.id);
        current_buffers[buffer.target] = buffer.id;
        ASSERT(!CheckGLError(), "Error binding buffer");
    }

    void gl_state_t::unbind_vao()
    {
        glBindVertexArray(0);
        current_vao = 0;
    }

    void gl_state_t::unbind_vbo()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        current_buffers[gl_array_buffer] = 0;
    }

    void gl_state_t::unbind_fbo()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        current_framebuffer = 0;
    }

    void gl_state_t::unbind_shader()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        current_shader = 0;
    }


    void gl_state_t::buffer_data(gl_buffer_object_t const& buffer, GLsizeiptr size, const GLvoid * data)
    {
        ASSERT(current_buffers[buffer.target] == buffer.id
            , "need to bind %s buffer %i before sending data", gl_buffer_target_strings[buffer.target], buffer.id);  /// FIXME check the right buffer, not just VBO. Requires I write code for gl_state_t to track it
        glBufferData(gl_buffer_target_enum_values[buffer.target], size, data, buffer.usage);
        ASSERT(!CheckGLError(), "error sending data to buffer");
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