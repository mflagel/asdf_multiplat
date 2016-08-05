#pragma once

#include <array>
#include <climits>
#include <memory>

#include <gl/glew.h>

#include "gl_enums.h"
#include "gl_vertex_spec.h"
#include "shader.h"

namespace asdf
{
    struct opengl_object_t
    {
        GLuint id = UINT_MAX;

        //virtual ~opengl_object_t() = 0;
    };

    struct vao_t : opengl_object_t
    {
        vao_t()
        {
            glGenVertexArrays(1, &id);
        }
        ~vao_t()
        {
            glDeleteVertexArrays(1, &id);
        }

        vao_t(const vao_t&) = delete;
        vao_t& operator=(const vao_t&) = delete;

        vao_t(vao_t&&) = default;
        vao_t& operator=(vao_t&&) = default;
    };

    struct framebuffer_object_t : opengl_object_t
    {
        framebuffer_object_t()
        {
            glGenFramebuffers(1, &id);
        }

        ~framebuffer_object_t()
        {
            glDeleteFramebuffers(1, &id);
        }

        framebuffer_object_t(const framebuffer_object_t&) = delete;
        framebuffer_object_t& operator=(const framebuffer_object_t&) = delete;

        framebuffer_object_t(framebuffer_object_t&&) = default;
        framebuffer_object_t& operator=(framebuffer_object_t&&) = default;
    };


    

    struct gl_buffer_object_t : opengl_object_t
    {
        const gl_buffer_targets_e target;
        GLenum usage = GL_DYNAMIC_DRAW;

        gl_buffer_object_t(gl_buffer_targets_e _target)
        : target(_target)
        {
            glGenBuffers(1, &id);
        }

        virtual ~gl_buffer_object_t()
        {
            glDeleteBuffers(1, &id);
        }

        gl_buffer_object_t(const gl_buffer_object_t&) = delete;
        gl_buffer_object_t& operator=(const gl_buffer_object_t&) = delete;

        gl_buffer_object_t(gl_buffer_object_t&&) = default;
        gl_buffer_object_t& operator=(gl_buffer_object_t&&) = default;
    };

    struct vbo_t : gl_buffer_object_t
    { vbo_t() : gl_buffer_object_t(gl_array_buffer) {} };

    struct tbo_t : gl_buffer_object_t
    { tbo_t() : gl_buffer_object_t(gl_texture_buffer) {} };

    struct ubo_t : gl_buffer_object_t
    { ubo_t() : gl_buffer_object_t(gl_uniform_buffer) {} };

    /*using vbo_t = gl_buffer_object_<gl_array_buffer>;
    using tbo_t = gl_buffer_object_<gl_texture_buffer>;
    using ubo_t = gl_buffer_object_<gl_uniform_buffer>;*/


    struct gl_renderable_t
    {
        vao_t vao;
        vbo_t vbo;
        size_t num_verts{0};
        GLuint draw_mode = GL_TRIANGLES;

        gl_renderable_t(){}
        virtual ~gl_renderable_t(){}

        gl_renderable_t(const gl_renderable_t&) = delete;
        gl_renderable_t& operator=(const gl_renderable_t&) = delete;

        gl_renderable_t(gl_renderable_t&&) = default;
        gl_renderable_t& operator=(gl_renderable_t&&) = default;
    };





    bool CheckShader(GLuint shader);
    bool CheckGLError(GLuint shader/* = 0xFFFFFFFF*/);
    bool CheckGLError();
}
