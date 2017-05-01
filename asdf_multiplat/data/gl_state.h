#pragma once

#include <stack>

#include "gl_resources.h"
#include "texture.h"

namespace asdf
{
    struct gl_state_t
    {
        bool initialized = false;

        std::vector<std::string> gl_extensions;
        GLint max_uniform_components = 0;
        size_t highest_glsl_version = 0;

        GLuint current_vao = 0;
        GLuint current_shader = 0;
        //GLuint current_framebuffer = 0;

        std::stack<std::pair<GLuint, gl_viewport_t>> fbo_stack;
        inline GLuint current_framebuffer() const { return fbo_stack.top().first; }
        inline gl_viewport_t const& current_viewport() const { return fbo_stack.top().second; };

        std::array<GLuint, gl_buffer_target_count> current_buffers;
        
        gl_state_t(){init_openGL();}
        void init_openGL();

        void bind(vao_t const&);
        void bind(std::shared_ptr<shader_t> const& shader);
        void bind(framebuffer_t const&);
        void bind(render_target_t const&);

        void bind(gl_buffer_object_t const&);

        GLuint current_vbo() const { return current_buffers[gl_array_buffer]; }

        void unbind_vao();
        void unbind_vbo();
        void unbind_fbo();
        void unbind_render_target();
        void unbind_shader();

        void push_fbo(GLuint fbo_id, gl_viewport_t const&);
        void push_fbo(framebuffer_t const& fbo, gl_viewport_t const&);
        void push_fbo(GLuint fbo_id, GLint x, GLint y, GLsizei width, GLsizei height);
        void push_fbo(framebuffer_t const&, GLint x, GLint y, GLsizei width, GLsizei height);
        void pop_fbo();

        void buffer_data(gl_buffer_object_t const& buffer, GLsizeiptr size, const GLvoid * data);
        void init_render_target(framebuffer_t const&, texture_t const&);

        bool assert_sync(); //ensures the values here are sync'd with openGL
    };

    /// GL State Proxy Object
    /// Allows code to access the gl state globally ex: GL_State->bind(something)
    struct gl_state_proxy_t
    {
        gl_state_proxy_t(){}

        gl_state_t* operator->();
    };
    extern gl_state_proxy_t GL_State;

    ///GL Utility Function Declarations
    size_t get_highest_glsl_ver();
    const/*expr*/ char* get_fbo_status_string(GLint status_code);
    const char* get_use_program_error(GLint error_code);

    ///RAII objects to manage the lifetime of fbo's and render targets
    struct scoped_fbo_t
    {
        scoped_fbo_t(GLuint fbo_id, GLint x, GLint y, GLsizei width, GLsizei height)
        {
            GL_State->push_fbo(fbo_id, x, y, width, height);
        }

        scoped_fbo_t(framebuffer_t const& fbo, GLint x, GLint y, GLsizei width, GLsizei height)
        {
            GL_State->push_fbo(fbo, x, y, width, height);
        }

        scoped_fbo_t(framebuffer_t const& fbo, gl_viewport_t v)
        {
            GL_State->push_fbo(fbo, v);
        }

        ~scoped_fbo_t()
        {
            GL_State->pop_fbo();
        }

        DELETE_COPY_ASSIGNMENT_MOVE(scoped_fbo_t);
    };

    struct scoped_render_target_t : scoped_fbo_t
    {
        scoped_render_target_t(render_target_t const& r)
        : scoped_fbo_t(r.fbo.id, 0, 0, r.texture.width, r.texture.height)
        {
        }

        ~scoped_render_target_t() = default;

        DELETE_COPY_ASSIGNMENT_MOVE(scoped_render_target_t);
    };
}