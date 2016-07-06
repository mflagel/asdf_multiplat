#pragma once

#include <gl\glew.h>
#include <climits>

namespace asdf
{
    struct vao_t
    {
        GLuint id = UINT_MAX;

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

        operator GLuint() const
        {
            return id;
        }
    };

    struct vbo_t
    {
        GLuint id = UINT_MAX;

        vbo_t()
        {
            glGenBuffers(1, &id);
        }
        ~vbo_t()
        {
            glDeleteBuffers(1, &id);
        }

        vbo_t(const vbo_t&) = delete;
        vbo_t& operator=(const vbo_t&) = delete;

        vbo_t(vbo_t&&) = default;
        vbo_t& operator=(vbo_t&&) = default;

        operator GLuint() const
        {
            return id;
        }
    };

    struct gl_renderable_t
    {
        vao_t vao;
        vbo_t vbo;
        size_t num_verts{0};
        GLuint draw_mode = GL_TRIANGLES;

        gl_renderable_t();
        ~gl_renderable_t();

        gl_renderable_t(const gl_renderable_t&) = delete;
        gl_renderable_t& operator=(const gl_renderable_t&) = delete;

        gl_renderable_t(gl_renderable_t&&) = default;
        gl_renderable_t& operator=(gl_renderable_t&&) = default;
    };
}
