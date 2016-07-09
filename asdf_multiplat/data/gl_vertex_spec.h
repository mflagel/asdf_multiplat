#pragma once

#include <gl\glew.h>

#include "shader.h"

namespace asdf
{
    namespace vertex_attrib
    {   
        template <size_t N, typename T, GLuint GL_Type>
        struct vertex_attrib_
        {
            static constexpr size_t num_components = N;
            static constexpr GLuint GL_Type = GL_Type;

            static constexpr size_t size_bytes()
            {
                return N * sizeof(T);
            }
        };

        /// Position
        template <size_t N, typename T, GLuint GL_Type>
        struct position_ : vertex_attrib_<N, T, GL_Type> 
        {
            static constexpr char* name = "VertexPosition";
        };

        template<size_t N>
        using position_f32_ = position_<N, float, GL_FLOAT>;
        template<size_t N>
        using position_i32_ = position_<N, int, GL_INT>;

        using position2_t = position_f32_<2>;
        using position3_t = position_f32_<3>;
        using position4_t = position_f32_<4>;

        using positioni2_t = position_i32_<2>;
        using positioni3_t = position_i32_<3>;
        using positioni4_t = position_i32_<4>;


        /// Color
        template <size_t N, typename T, GLuint GL_Type>
        struct color_ : vertex_attrib_<N, T, GL_Type> 
        {
            static constexpr char* name = "VertexColor";
        };

        template <size_t N>
        using colorf_ = color_<N, float, GL_FLOAT>;

        using color_t = colorf_<4>;
    }


    template <typename... Attributes>
    struct gl_vertex_spec_
    {
        std::tuple<Attributes...> vertex_attribs;  //ideally this would be static, but I can't get it to work

        size_t get_stride() const
        {
            size_t stride = 0;
            for_each(vertex_attribs, [&stride](auto const& attrib)
            {
                int asdf = 9001;
                ++asdf;
                stride += attrib.size_bytes();
            });

            return stride;
        }

        // assumes attribs are interlaced, and vertex data is sent as an array of structs
        void set_vertex_attribs(std::shared_ptr<shader_t >const& shader) const
        {
            size_t stride = get_stride();
            size_t offset = 0;
            for_each(vertex_attribs, [&offset, stride](auto const& attrib)
            {
                GLint attrib_loc = glGetAttribLocation(shader->shader_program_id, attrib.name);
                glEnableVertexAttribArray(attrib_loc);
                glVertexAttribPointer(attrib_loc, attrib.num_components, attrib.GL_Type, stride, offset);

                offset += attrib.size_bytes(); //next attrib will be this many bytes next
            });
        }
    };
}
