#pragma once

#include <memory>

#include <gl/glew.h>

#include "main/asdf_defs.h"
#include "stdafx.h"
#include "utilities/utilities.h"
#include "shader.h"

DIAGNOSTIC_PUSH
DIAGNOSTIC_IGNORE(-Wwrite-strings)

namespace asdf
{
    namespace vertex_attrib
    {   
        template <size_t N, typename T, GLuint _GL_Type>
        struct vertex_attrib_
        {
            static constexpr size_t num_components = N;
            static constexpr GLuint GL_Type = _GL_Type;

            static constexpr size_t size_bytes()
            {
                return N * sizeof(T);
            }
        };


#define VERTEX_ATTRIB_NAMED(struct_name, attrib_name)         \
        template <size_t N, typename T, GLuint GL_Type>       \
        struct struct_name : vertex_attrib_<N, T, GL_Type>    \
        {                                                     \
            static constexpr char* name = #attrib_name; \
        };                                                    \
        template <size_t N, typename T, GLuint GL_Type> constexpr char* struct_name<N, T, GL_Type>::name; //apparently clang needs this
        //----


        

        /// Position
        VERTEX_ATTRIB_NAMED(position_, VertexPosition);

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


        // /// Normal
        VERTEX_ATTRIB_NAMED(normal_, VertexNormal);

        template<size_t N>
        using normal_f32_ = normal_<N, float, GL_FLOAT>;

        using normal2_t = normal_f32_<2>;
        using normal3_t = normal_f32_<3>;


        /// Color
        template <size_t N, typename T, GLuint GL_Type>
        struct color_ : vertex_attrib_<N, T, GL_Type> 
        {
            static constexpr char* name = "VertexColor";
        };
        template <size_t N, typename T, GLuint GL_Type> constexpr char* color_<N, T, GL_Type>::name;  //apparently clang needs this to link properly

        template <size_t N>
        using colorf_ = color_<N, float, GL_FLOAT>;

        using color_t = colorf_<4>;


        /// Texture Coords
        template <typename T, GLuint GL_Type>
        struct texture_coords_ : vertex_attrib_<2, T, GL_Type> 
        {
            static constexpr char* name = "TextureCoords";
        };
        template <typename T, GLuint GL_Type> constexpr char* texture_coords_<T, GL_Type>::name;

        using texture_coords_t = texture_coords_<float, GL_FLOAT>;

    } // end namespace vertex_attrib


    //defined in gl_resources.cpp so I can access CheckGLError()
    bool check_attrib_error();


    template <typename... Attributes>
    struct gl_vertex_spec_
    {
        std::tuple<Attributes...> vertex_attribs;  //ideally this would be static, but I can't get it to work

        size_t get_stride() const
        {
            size_t stride = 0;
            util::for_each(vertex_attribs, [&stride](auto const& attrib)
            {
                stride += attrib.size_bytes();
            });

            return stride;
        }

        // assumes attribs are interlaced, and vertex data is sent as an array of structs
        void set_vertex_attribs(std::shared_ptr<shader_t >const& shader) const
        {
            ASSERT(shader, "setting vert attribs without a shader");

            size_t stride = get_stride();
            size_t offset = 0;

            util::for_each(vertex_attribs, [&shader, &offset, stride](auto const& attrib)
            {
                ASSERT(shader->attributes.count(attrib.name) == 1
                    , "attribute %s not found in shader %s (%d)"
                    , attrib.name, shader->name.c_str(), shader->shader_program_id);

                auto attrib_loc = shader->attributes[attrib.name];
                glEnableVertexAttribArray(attrib_loc);
                glVertexAttribPointer(attrib_loc, attrib.num_components, attrib.GL_Type, GL_FALSE, stride, reinterpret_cast<GLvoid*>(offset)); //GL_FALSE is for fixed-point data value normalization

                offset += attrib.size_bytes(); //next attrib will be this many bytes next
            });
        }
    };
}

DIAGNOSTIC_POP
