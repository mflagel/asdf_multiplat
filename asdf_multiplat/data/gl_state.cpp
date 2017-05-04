#include "stdafx.h"
#include "gl_state.h"

#include <algorithm>

#include "main/asdf_multiplat.h"

namespace asdf
{
    gl_state_proxy_t GL_State;
    
    gl_state_t* gl_state_proxy_t::operator->()
    {
        ASSERT(current_state_machine, "Current GL State is null");
        return current_state_machine;
    }

    void gl_state_proxy_t::set_current_state_machine(gl_state_t& state_machine)
    {
        current_state_machine = &state_machine;
    }

    using namespace util;

    gl_state_t::gl_state_t(void* _gl_context)
    : gl_context(_gl_context)
    {
        ASSERT(gl_context, "GL_State given a null context");
        init_openGL();
    }

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
        ASSERT(!CheckGLError(), "Error before binding shader");
        //LOG_IF(current_shader == shader->shader_program_id, "shader \'%s\' already in use", shader->name.c_str());
        glUseProgram(shader->shader_program_id);
        current_shader = shader->shader_program_id;

#if _DEBUG
        GLenum errCode = GL_NO_ERROR;
        while((errCode = glGetError()) != GL_NO_ERROR)
        {
            ASSERT(errCode == GL_NO_ERROR, "Error binding shader %s\n%s", shader->name.c_str(), get_use_program_error(errCode));
        }
#endif
    }

    void gl_state_t::bind(framebuffer_t const& fbo)
    {
        ASSERT(!fbo_stack.empty(), "");

        //overwrite top of stack's FBO id (re-use existing viewport)
        auto& f_v = fbo_stack.top();
        f_v.first = fbo.id;
        glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
    }

    void gl_state_t::bind(render_target_t const& render_target)
    {
        if(fbo_stack.empty())
        {
            push_fbo(render_target.fbo.id, 0, 0, render_target.texture.width, render_target.texture.height);
        }
        else
        {
            auto& f_v = fbo_stack.top();
            f_v.first = render_target.fbo.id;
            f_v.second = gl_viewport_t{glm::ivec2(0, 0), glm::uvec2(render_target.texture.width, render_target.texture.height)};
        }
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
        while(!fbo_stack.empty())
            fbo_stack.pop();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // current_framebuffer = 0;
    }

    void gl_state_t::unbind_render_target()
    {
        unbind_fbo();
        pop_fbo();
    }

    void gl_state_t::unbind_shader()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        current_shader = 0;
    }


    void gl_state_t::push_fbo(GLuint fbo_id, gl_viewport_t const& v)
    {
        fbo_stack.push(std::make_pair(std::ref(fbo_id), v));
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

        ASSERT(!CheckGLError(), "GL Error binding framebuffer %i", fbo_id);
        glViewport(v.bottom_left.x, v.bottom_left.y, v.size.x, v.size.y);
        ASSERT(!CheckGLError(), "GL Error setting viewport to %i,%i %i,%i", v.bottom_left.x, v.bottom_left.y, v.size.x, v.size.y);
    }

    void gl_state_t::push_fbo(framebuffer_t const& fbo, gl_viewport_t const& v)
    {
        push_fbo(fbo.id, v);
    }

    void gl_state_t::push_fbo(GLuint fbo_id, GLint x, GLint y, GLsizei width, GLsizei height)
    {
        push_fbo(fbo_id, gl_viewport_t{glm::ivec2{x,y}, glm::uvec2{width,height}});
    }

    void gl_state_t::push_fbo(framebuffer_t const& fbo, GLint x, GLint y, GLsizei width, GLsizei height)
    {
        push_fbo(fbo.id, x, y, width, height);
    }

    void gl_state_t::pop_fbo()
    {
        ASSERT(!fbo_stack.empty(), "");
        fbo_stack.pop();

        if(!fbo_stack.empty())
        {
            auto const& v = fbo_stack.top().second;
            glViewport(v.bottom_left.x, v.bottom_left.y, v.size.x, v.size.y);
            ASSERT(!CheckGLError(), "GL Error setting viewport");
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_stack.top().first);

            ASSERT(!CheckGLError(), "GL Error Popping FBO to %d", fbo_stack.top().first);
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            ASSERT(!CheckGLError(), "GL Error Popping FBO to 0");
        }
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
        scoped_fbo_t scoped(fbo, 0, 0, texture.width, texture.height);

        glFramebufferTexture2D(GL_FRAMEBUFFER          // this is the only value allowed
                             , GL_COLOR_ATTACHMENT0    // use GL_COLOR_ATTACHMENT0 to render color data (as opposed to depth or stencil)
                             , GL_TEXTURE_2D           // currently texture_t always uses GL_TEXTURE_2D
                             , texture.texture_id      // self-explanetory
                             , 0);                     // always 0 for the 0th mipmap level

        // only care about rendering into one draw buffer
        GLenum draw_buffers = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &draw_buffers);

        auto gl_fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        ASSERT(gl_fbo_status == GL_FRAMEBUFFER_COMPLETE, "Broken FBO %i  Status: 0x%0x\nError: %s", fbo.id, gl_fbo_status, get_fbo_status_string(gl_fbo_status));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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


    /// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCheckFramebufferStatus.xhtml
    const/*expr*/ char* get_fbo_status_string(GLint status_code)
    {
        switch(status_code)
        {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            return "GL_FRAMEBUFFER_COMPLETE";

        //errors
        case GL_FRAMEBUFFER_UNDEFINED:
            return "GL_FRAMEBUFFER_UNDEFINED : the specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist";
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT : one of the framebuffer attachment points are framebuffer incomplete";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT : the framebuffer does not have at least one image attached to it";
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER : the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi";
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER : GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER";
        case GL_FRAMEBUFFER_UNSUPPORTED:
            return "GL_FRAMEBUFFER_UNSUPPORTED : the combination of internal formats of the attached images violates an implementation-dependent set of restrictions";
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE : One or both of the following\nthe value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES\nthe value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures";
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS : one of the framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target";


        default:
            return "Unknown FBO Status Code";
        };
    }

    /// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glUseProgram.xml
    const char* get_use_program_error(GLint error_code)
    {
        switch(error_code)
        {
        case GL_INVALID_VALUE:
            return "program is neither 0 nor a value generated by OpenGL";
        case GL_INVALID_OPERATION:
            return "one or more of:\n program is not a program object\n program could not be made part of current state\n";
        default:
            return "unknown error";
        }
    }
}
