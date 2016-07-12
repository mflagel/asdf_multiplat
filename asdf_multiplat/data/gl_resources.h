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

    template <gl_buffer_targets_e BufferTarget>
    struct gl_buffer_object_ : opengl_object_t
    {
        GLenum usage = GL_DYNAMIC_DRAW;

        gl_buffer_object_()
        {
            glGenBuffers(1, &id);
        }

        ~gl_buffer_object_()
        {
            glDeleteBuffers(1, &id);
        }

        gl_buffer_object_(const gl_buffer_object_&) = delete;
        gl_buffer_object_& operator=(const gl_buffer_object_&) = delete;

        gl_buffer_object_(gl_buffer_object_&&) = default;
        gl_buffer_object_& operator=(gl_buffer_object_&&) = default;

        void buffer_data(GLsizeiptr size, const GLvoid * data)
        {
            ASSERT(GL_State.current_vbo == id, "need to bind this buffer before sending data");  /// FIXME check the right buffer, not just VBO. Requires I write code for gl_state_t to track it
            glBufferData(gl_buffer_target_enum_values[BufferTarget], size, data, usage);
        }

        void buffer_data(GLsizeiptr size, const GLvoid * data, GLenum _usage)
        {
            usage = _usage;
            buffer_data(size, data);
        }
    };

    using vbo_t = gl_buffer_object_<gl_array_buffer>;
    using tbo_t = gl_buffer_object_<gl_texture_buffer>;
    using ubo_t = gl_buffer_object_<gl_uniform_buffer>;


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
        GLuint current_vbo = 0; //todo: keep track of what buffer is bound to each buffer target (vertex, uniform, etc)
        GLuint current_shader = 0;


        void init_openGL();

        void bind(vao_t const&);
        void bind(vbo_t const&);
        void bind(std::shared_ptr<shader_t> const& shader);

        void unbind_vao();
        void unbind_vbo();
        void unbind_shader();

        bool assert_sync(); //ensures the values here are sync'd with openGL
    };

    bool CheckShader(GLuint shader);
    bool CheckGLError(GLuint shader/* = 0xFFFFFFFF*/);
    bool CheckGLError();




    extern gl_state_t GL_State;
}
