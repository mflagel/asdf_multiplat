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

            GL_State->bind(vao);
            glDrawArrays(_draw_mode, 0, num_verts);
            GL_State->unbind_vao();
        }
    };

	//polygon_t polygon_rect(glm::vec2 size, const color_t& color);
	//polygon_t polygon_circle(float radius, const color_t& color, size_t num_facets = 40);
}