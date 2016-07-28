#pragma once

#include <array>
#include <climits>
#include <memory>

#include <gl/glew.h>

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


    enum gl_buffer_targets_e : uint32_t
    {
          gl_array_buffer = 0
        , gl_atomic_counter_buffer
        , gl_copy_read_buffer
        , gl_copy_write_buffer
        , gl_dispatch_indirect_buffer
        , gl_draw_indirect_buffer
        , gl_element_array_buffer
        , gl_pixel_pack_buffer
        , gl_pixel_unpack_buffer
        , gl_query_buffer
        , gl_shader_storage_buffer
        , gl_texture_buffer
        , gl_transform_feedback_buffer
        , gl_uniform_buffer
        , gl_buffer_target_count
    };

    constexpr std::array<GLenum, gl_buffer_target_count> gl_buffer_target_enum_values
    {
          GL_ARRAY_BUFFER                // Vertex attributes
        , GL_ATOMIC_COUNTER_BUFFER       // Atomic counter storage
        , GL_COPY_READ_BUFFER            // Buffer copy source
        , GL_COPY_WRITE_BUFFER           // Buffer copy destination
        , GL_DISPATCH_INDIRECT_BUFFER    // Indirect compute dispatch commands
        , GL_DRAW_INDIRECT_BUFFER        // Indirect command arguments
        , GL_ELEMENT_ARRAY_BUFFER        // Vertex array indices
        , GL_PIXEL_PACK_BUFFER           // Pixel read target
        , GL_PIXEL_UNPACK_BUFFER         // Texture data source
        , GL_QUERY_BUFFER                // Query result buffer
        , GL_SHADER_STORAGE_BUFFER       // Read-write storage for shaders
        , GL_TEXTURE_BUFFER              // Texture data buffer
        , GL_TRANSFORM_FEEDBACK_BUFFER   // Transform feedback buffer
        , GL_UNIFORM_BUFFER              // Uniform block storage
    };

    constexpr std::array<char*, gl_buffer_target_count> gl_buffer_target_strings =
    {
          "GL_ARRAY_BUFFER"
        , "GL_ATOMIC_COUNTER_BUFFER"
        , "GL_COPY_READ_BUFFER"
        , "GL_COPY_WRITE_BUFFER"
        , "GL_DISPATCH_INDIRECT_BUFFER"
        , "GL_DRAW_INDIRECT_BUFFER"
        , "GL_ELEMENT_ARRAY_BUFFER"
        , "GL_PIXEL_PACK_BUFFER"
        , "GL_PIXEL_UNPACK_BUFFER"
        , "GL_QUERY_BUFFER"
        , "GL_SHADER_STORAGE_BUFFER"
        , "GL_TEXTURE_BUFFER"
        , "GL_TRANSFORM_FEEDBACK_BUFFER"
        , "GL_UNIFORM_BUFFER"
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

        ~gl_buffer_object_t()
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



    struct gl_state_t
    {
        bool initialized = false;

        std::vector<std::string> gl_extensions;
        GLint max_uniform_components = 0;

        GLuint current_vao = 0;
        GLuint current_shader = 0;

        std::array<GLuint, gl_buffer_target_count> current_buffers;
        

        void init_openGL();

        void bind(vao_t const&);
        void bind(std::shared_ptr<shader_t> const& shader);

        void bind(gl_buffer_object_t const&);

        GLuint current_vbo() const { return current_buffers[gl_array_buffer]; }

        void unbind_vao();
        void unbind_vbo();
        void unbind_shader();

        void buffer_data(gl_buffer_object_t const& buffer, GLsizeiptr size, const GLvoid * data);

        bool assert_sync(); //ensures the values here are sync'd with openGL
    };

    extern gl_state_t GL_State;

    bool CheckShader(GLuint shader);
    bool CheckGLError(GLuint shader/* = 0xFFFFFFFF*/);
    bool CheckGLError();

    namespace gl
    {
        enum types_e
        {
              gl_none                = GL_NONE
            , gl_signed_normalized   = GL_SIGNED_NORMALIZED
            , gl_unsigned_normalized = GL_UNSIGNED_NORMALIZED
            , gl_float               = GL_FLOAT
            , gl_int                 = GL_INT
            , gl_unsigned_int        = GL_UNSIGNED_INT
        };

        // used in glGetTexLevelParameteriv
        enum channel_types_e
        {
              texture_red_type    = GL_TEXTURE_RED_TYPE
            , texture_green_type  = GL_TEXTURE_GREEN_TYPE
            , texture_blue_type   = GL_TEXTURE_BLUE_TYPE
            , texture_alpha_type  = GL_TEXTURE_ALPHA_TYPE
            , texture_depth_type  = GL_TEXTURE_DEPTH_TYPE
        };
    }

}
