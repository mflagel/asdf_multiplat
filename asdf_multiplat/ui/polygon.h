#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "data/gl_state.h"

namespace asdf
{
	struct  polygon_vertex_t
	{
        static gl_vertex_spec_<vertex_attrib::position3_t, vertex_attrib::color_t> vertex_spec;

		glm::vec3   position;
		glm::vec4   color;
	};
	
	using polygon_t = std::vector<polygon_vertex_t>;


    template <typename VertexType>
    using polygon_ = std::vector<VertexType>;


    /// TODO  I should probably rename this to something more generalized
    ///       since 'polygon' is not really the best word
    ///       ex: A mesh is a set of connected polygons that get rendered
    ///       but this doesnt really store a mesh since there are no indices
    ///       Maybe this should be named gl_renderable_<>?
    /// Potential Name: renderable_geometry_<>

    template <typename VertexType>
    struct rendered_polygon_ : gl_renderable_t
    {
        bool initialized = false;

        rendered_polygon_()
        {
        }

        void initialize(std::shared_ptr<shader_t> const& shader)
        {
            LOG_IF(initialized, "WARNING: double-initializing polygon");

            GL_State->bind(vao);

            GL_State->bind(vbo);
            VertexType::vertex_spec.set_vertex_attribs(shader);

            GL_State->unbind_vao();
            GL_State->unbind_vbo();  //unbind vbo after vao so that the vao knows to use the vbo

            initialized = true;
        }

        void set_data(polygon_<VertexType> const& verts)
        {
            set_data(verts.data(), verts.size());
        }

        void set_data(const VertexType* verts, size_t n)
        {
            buffer_data(verts, n);
        }

        void buffer_data(const VertexType* verts, size_t n)
        {
            LOG_IF(CheckGLError(), "Error before rendered_polygon_::set_data()");

            GL_State->bind(vbo);
            GL_State->buffer_data(vbo, n * sizeof(VertexType), static_cast<const void*>(verts));
            GL_State->unbind_vbo();  //unbind vbo after vao so that the vao knows to use the vbo

            num_verts = n;

            LOG_IF(CheckGLError(), "Error during rendered_polygon_::set_data()");
        }

        void render() const
        {
            render(draw_mode);
        }

        void render(GLuint _draw_mode) const
        {
            ASSERT(initialized, "Rendering polygon before vao is set up with initialize()");

            if(num_verts > 0)
            {
                GL_State->bind(vao);
                glDrawArrays(_draw_mode, 0, num_verts);
                GL_State->unbind_vao();
            }
        }
    };

    // stores multiple meshes in one vbo and uses glMultiDrawArrays
    template <typename VertexType>
    struct rendered_multi_polygon_ : rendered_polygon_<VertexType>
    {
        std::vector<GLint> first_vert_indices;
        std::vector<GLsizei> vert_counts;
        size_t total_vertex_count;

        size_t num_sub_meshes() const { return first_vert_indices.size(); }

        using rendered_polygon_<VertexType>::buffer_data;

        void set_data(polygon_<VertexType> const& verts)
        {
            set_data(verts.data(), verts.size());
        }

        void set_data(const VertexType* verts, size_t n)
        {
            rendered_polygon_<VertexType>::set_data(verts, n);

            first_vert_indices.resize(1);
            first_vert_indices[0] = 0;
            vert_counts.resize(1);
            vert_counts[0] = n;
            total_vertex_count = n;
        }

        void set_data(std::vector<polygon_<VertexType>> const& polygons)
        {
            LOG_IF(CheckGLError(), "Error before rendered_multi_polygon_::set_data()");

            first_vert_indices.resize(polygons.size());
            vert_counts.resize(polygons.size());

            // accounting
            total_vertex_count = 0;
            for(size_t i = 0; i < polygons.size(); ++i)
            {
                first_vert_indices[i] = total_vertex_count;
                vert_counts[i] = polygons[i].size();
                total_vertex_count += polygons[i].size();
            }


            ///  Because a vector of vectors is not entirely contiguous
            ///  I'll have to allocate enough VBO space to fit everything
            ///  and then use glBufferSubData to send each polygon's vertices

            GL_State->bind(gl_renderable_t::vbo);
            GLsizeiptr bufsize = total_vertex_count * sizeof(VertexType);
            GL_State->buffer_data(gl_renderable_t::vbo, bufsize, nullptr);  //nullptr data to reserve space without copying

            LOG_IF(CheckGLError(), "Error pre-allocating vertex buffer in rendered_multi_polygon_::set_data()");

            for(size_t i = 0; i < polygons.size(); ++i)
            {
                auto vert_size_bytes = sizeof(VertexType);
                auto       target = gl_buffer_target_enum_values[gl_renderable_t::vbo.target];
                GLintptr   offset = first_vert_indices[i] * vert_size_bytes; //offset in bytes
                GLsizeiptr size   = vert_counts[i] * vert_size_bytes;        //size in bytes

                glBufferSubData(target, offset, size, static_cast<const void*>(polygons[i].data()));
            }

            GL_State->unbind_vbo();


            LOG_IF(CheckGLError(), "Error during rendered_multi_polygon_::set_data()");
        }

        void render() const
        {
            render(gl_renderable_t::draw_mode);
        }

        void render(GLuint _draw_mode) const
        {
            ASSERT(rendered_polygon_<VertexType>::initialized, "Rendering multi_polygon before vao is set up with initialize()");
            ASSERT(first_vert_indices.size() == vert_counts.size(), "");

            if(first_vert_indices.size() > 0)
            {
                GL_State->bind(gl_renderable_t::vao);
                glMultiDrawArrays(_draw_mode, first_vert_indices.data(), vert_counts.data(), num_sub_meshes());
                GL_State->unbind_vao();
            }
        }
    };

	//polygon_t polygon_rect(glm::vec2 size, const color_t& color);
	//polygon_t polygon_circle(float radius, const color_t& color, size_t num_facets = 40);
}