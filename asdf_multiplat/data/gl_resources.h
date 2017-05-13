#pragma once

#include <array>
#include <climits>
#include <memory>

#include <gl/glew.h>

#include "gl_enums.h"
#include "gl_vertex_spec.h"
#include "shader.h"
#include "texture.h"

namespace asdf
{

    struct opengl_object_t
    {
        GLuint id = UINT_MAX;

        opengl_object_t() = default;
        //virtual ~opengl_object_t() = 0;
        virtual ~opengl_object_t() = default;
    };

    #define DELETE_COPY_ASSIGNMENT_MOVE(_obj_name_) \
        _obj_name_(const _obj_name_&) = delete; \
        _obj_name_& operator=(const _obj_name_&) = delete; \
        _obj_name_(_obj_name_&&) = delete; \
        _obj_name_& operator=(_obj_name_&&) = delete;
    /*--------*/


    #define GL_OBJ(_obj_name_, _gen_func_, _delete_func_)   \
        struct _obj_name_ : opengl_object_t                 \
        {                                                   \
            _obj_name_()                                    \
            {                                               \
                _gen_func_(1, &id);                         \
            }                                               \
                                                            \
            ~_obj_name_()                                   \
            {                                               \
                _delete_func_(1, &id);                      \
            }                                               \
                                                            \
            DELETE_COPY_ASSIGNMENT_MOVE(_obj_name_)         \
        };                                                  
    /*--------*/

    GL_OBJ(vao_t, glGenVertexArrays, glDeleteVertexArrays);
    GL_OBJ(framebuffer_t, glGenFramebuffers, glDeleteFramebuffers);
    GL_OBJ(render_buffer_t, glGenRenderbuffers, glDeleteRenderbuffers);

    struct render_target_t
    {
        framebuffer_t fbo;
        texture_t texture;

        render_target_t(int width, int height);
        void init();
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

        DELETE_COPY_ASSIGNMENT_MOVE(gl_buffer_object_t);
    };

    struct vbo_t : gl_buffer_object_t
    { vbo_t() : gl_buffer_object_t(gl_array_buffer) {} };

    struct tbo_t : gl_buffer_object_t
    { tbo_t() : gl_buffer_object_t(gl_texture_buffer) {} };

    struct ubo_t : gl_buffer_object_t
    { ubo_t() : gl_buffer_object_t(gl_uniform_buffer) {} };



    struct gl_renderable_t
    {
        vao_t vao;
        vbo_t vbo;
        size_t num_verts{0};
        GLuint draw_mode = GL_TRIANGLES;

        gl_renderable_t(){}
        virtual ~gl_renderable_t(){}

        DELETE_COPY_ASSIGNMENT_MOVE(gl_renderable_t);
    };


    struct gl_viewport_t
    {
        glm::ivec2 bottom_left;
        glm::uvec2 size;
    };


    bool CheckShader(GLuint shader);
    bool CheckGLError(GLuint shader/* = 0xFFFFFFFF*/);
    bool CheckGLError();
}
