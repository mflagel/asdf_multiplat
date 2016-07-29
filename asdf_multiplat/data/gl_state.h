#pragma once

#include "gl_resources.h"

namespace asdf
{
    struct gl_state_t
    {
        bool initialized = false;

        std::vector<std::string> gl_extensions;
        GLint max_uniform_components = 0;

        GLuint current_vao = 0;
        GLuint current_shader = 0;
        GLuint current_framebuffer = 0;

        std::array<GLuint, gl_buffer_target_count> current_buffers;
        

        void init_openGL();

        void bind(vao_t const&);
        void bind(std::shared_ptr<shader_t> const& shader);
        void bind(framebuffer_object_t const&);

        void bind(gl_buffer_object_t const&);

        GLuint current_vbo() const { return current_buffers[gl_array_buffer]; }

        void unbind_vao();
        void unbind_vbo();
        void unbind_fbo();
        void unbind_shader();

        void buffer_data(gl_buffer_object_t const& buffer, GLsizeiptr size, const GLvoid * data);

        bool assert_sync(); //ensures the values here are sync'd with openGL
    };

    extern gl_state_t GL_State;
}