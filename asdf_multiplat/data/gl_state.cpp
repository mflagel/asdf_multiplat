#include "stdafx.h"
#include "gl_state.h"

#include <algorithm>

#include "main/asdf_multiplat.h"

namespace asdf
{
    gl_state_proxy_t GL_State;
    
    gl_state_t* gl_state_proxy_t::operator->()
    {
        return &(app.renderer->gl_state);
    }

    using namespace util;

    void gl_state_t::init_openGL()
    {
        GLenum err = glewInit();
        ASSERT(err == GLEW_OK, "Error: %s\n", glewGetErrorString(err));
        ASSERT(!CheckGLError(), "OpenGL Error after glewInit");

        LOG("### Using OpenGL Version: %s ###\n\n", glGetString(GL_VERSION));

        highest_glsl_version = get_highest_glsl_ver();
        LOG("### Highest Supported GLSL Version: %zu", highest_glsl_version);

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
        //LOG_IF(current_vao == vao.id, "vao %i already in use", vao.id);
        glBindVertexArray(vao.id);
        current_vao = vao.id;
    }

    void gl_state_t::bind(std::shared_ptr<shader_t> const& shader)
    {
        //LOG_IF(current_shader == shader->shader_program_id, "shader \'%s\' already in use", shader->name.c_str());
        glUseProgram(shader->shader_program_id);
        current_shader = shader->shader_program_id;
    }

    void gl_state_t::bind(framebuffer_t const& fbo)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
        current_framebuffer = fbo.id;
    }

    void gl_state_t::bind(gl_buffer_object_t const& buffer)
    {
        //LOG_IF(current_buffers[buffer.target] == buffer.id, "buffer  %i already in use for target %s", buffer.id, gl_buffer_target_strings[buffer.target]);

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

    void gl_state_t::init_render_target(framebuffer_t const& fbo, texture_t const& texture)
    {
        auto prev_fbo = current_framebuffer;
        bind(fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER          // this is the only value allowed
                             , GL_COLOR_ATTACHMENT0    // use GL_COLOR_ATTACHMENT0 to render color data (as opposed to depth or stencil)
                             , GL_TEXTURE_2D           // currently texture_t always uses GL_TEXTURE_2D
                             , texture.texture_id      // self-explanetory
                             , 0);                     // always 0 for the 0th mipmap level

        // only care about rendering into one draw buffer
        GLenum draw_buffers = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &draw_buffers);

        auto gl_fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        ASSERT(gl_fbo_status == GL_FRAMEBUFFER_COMPLETE, "GL Error initializing framebuffer %d for render target (texture) %d. FBO Status: 0x%0x", fbo.id, texture.texture_id, gl_fbo_status);

        glViewport(0, 0, texture.width, texture.height);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
        current_framebuffer = prev_fbo;
    }


    bool gl_state_t::assert_sync()
    {
        //todo:
        //check vbo
        //check vao
        //check shader

        return true;
    }


    // currently support glsl 130 and 330
    size_t get_highest_glsl_ver()
    {
        size_t ver = 0;
        // ver = std::max(ver, size_t(110 * GLEW_VERSION_2_0));
        // ver = std::max(ver, size_t(120 * GLEW_VERSION_2_1));
        ver = std::max(ver, size_t(130 * GLEW_VERSION_3_0));
        // ver = std::max(ver, size_t(140 * GLEW_VERSION_3_1));
        // ver = std::max(ver, size_t(150 * GLEW_VERSION_3_2));
        ver = std::max(ver, size_t(330 * GLEW_VERSION_3_3));
        // ver = std::max(ver, size_t(400 * GLEW_VERSION_4_0));
        // ver = std::max(ver, size_t(410 * GLEW_VERSION_4_1));
        // ver = std::max(ver, size_t(420 * GLEW_VERSION_4_2));
        // ver = std::max(ver, size_t(430 * GLEW_VERSION_4_3));
        // ver = std::max(ver, size_t(440 * GLEW_VERSION_4_4));
        // ver = std::max(ver, size_t(450 * GLEW_VERSION_4_5));

        return ver;
    }
}