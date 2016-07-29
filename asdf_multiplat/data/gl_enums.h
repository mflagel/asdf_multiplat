#pragma once

#include <array>

#include <gl/glew.h>

namespace asdf
{
    /************************
     * BUFFER TARGETS
     ************************/
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


    /************************
     * TEXTURES
     ************************/
    /*enum gl_texture_targets_e 
    {
          gl_texture_1d = 0
        , gl_texture_2d
        , gl_texture_3d
        , gl_texture_rectangle
        , gl_texture_buffer      // need to avoid name collision with gl_buffer_targets_e::gl_texture_buffer
        , gl_texture_cube_map
        , gl_texture_1d_array
        , gl_texture_2d_array
        , gl_texture_cube_map_array
        , gl_texture_2d_multisample
        , gl_texture_2d_multisample_array
        , gl_texture_targets_count
    };

    constexpr std::array<GLuint, gl_texture_targets_count> gl_texture_targets =
    {
          GL_TEXTURE_1D                   // Images in this texture all are 1-dimensional. They have width, but no height or depth.
        , GL_TEXTURE_2D                   // Images in this texture all are 2-dimensional. They have width and height, but no depth.
        , GL_TEXTURE_3D                   // Images in this texture all are 3-dimensional. They have width, height, and depth.
        , GL_TEXTURE_RECTANGLE            // The image in this texture (only one image. No mipmapping) is 2-dimensional. Texture coordinates used for these textures are not normalized.
        , GL_TEXTURE_BUFFER               // The image in this texture (only one image. No mipmapping) is 1-dimensional. The storage for this data comes from a Buffer Object.
        , GL_TEXTURE_CUBE_MAP             // There are exactly 6 distinct sets of 2D images, all of the same size. They act as 6 faces of a cube.
        , GL_TEXTURE_1D_ARRAY             // Images in this texture all are 1-dimensional. However, it contains multiple sets of 1-dimensional images, all within one texture. The array length is part of the texture's size.
        , GL_TEXTURE_2D_ARRAY             // Images in this texture all are 2-dimensional. However, it contains multiple sets of 2-dimensional images, all within one texture. The array length is part of the texture's size.
        , GL_TEXTURE_CUBE_MAP_ARRAY       // Images in this texture are all cube maps. It contains multiple sets of cube maps, all within one texture. The array length * 6 (number of cube faces) is part of the texture size.
        , GL_TEXTURE_2D_MULTISAMPLE       // The image in this texture (only one image. No mipmapping) is 2-dimensional. Each pixel in these images contains multiple samples instead of just one value.
        , GL_TEXTURE_2D_MULTISAMPLE_ARRAY // Combines 2D array and 2D multisample types. No mipmapping.
    };*/

    /************************
     * OTHER
    *************************/
    enum gl_types_e
    {
          gl_none                = GL_NONE
        , gl_signed_normalized   = GL_SIGNED_NORMALIZED
        , gl_unsigned_normalized = GL_UNSIGNED_NORMALIZED
        , gl_float               = GL_FLOAT
        , gl_int                 = GL_INT
        , gl_unsigned_int        = GL_UNSIGNED_INT
    };

    // used in glGetTexLevelParameteriv
    enum gl_channel_types_e
    {
        gl_texture_red_type = 0
        , gl_texture_green_type
        , gl_texture_blue_type
        , gl_texture_alpha_type
        , gl_texture_depth_type
        , gl_channel_types_count
    };

    constexpr std::array<GLuint, gl_channel_types_count> gl_channel_types =
    {
          GL_TEXTURE_RED_TYPE
        , GL_TEXTURE_GREEN_TYPE
        , GL_TEXTURE_BLUE_TYPE
        , GL_TEXTURE_ALPHA_TYPE
        , GL_TEXTURE_DEPTH_TYPE
    };
}