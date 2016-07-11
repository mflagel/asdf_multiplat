#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "ui/ui_base.h"
#include "data/gl_resources.h"

namespace asdf
{
	struct  polygon_vertex_t
	{
		glm::vec3   position;
		color_t     color;
	};
	
	using polygon_t = std::vector<polygon_vertex_t>;

	struct ui_polygon_t : ui_base_t
	{
		vao_t vao;
		vbo_t vbo;
		size_t num_verts{0};
		GLuint draw_mode = GL_TRIANGLES;
		//---

	    ui_polygon_t();
	    ui_polygon_t(polygon_t const& points);

        ui_polygon_t(const ui_polygon_t&) = delete;
	    ui_polygon_t operator=(const ui_polygon_t&) = delete;

	    void set_data(polygon_t const& points);

	    // void render(glm::vec3 const& _position, glm::mat3 const& _matrix, color_t const& _color);
	    void render();
	};

    template <typename VertexType>
    using polygon_ = std::vector<VertexType>;

    template <typename VertexType>
    struct rendered_polygon_ : gl_renderable_t
    {
        rendered_polygon_()
        {}
        rendered_polygon_(polygon_<VertexType> const& verts)
        {}

        void set_vertex_attribs(std::shared_ptr<shader_t> const& shader)
        {
            size_t stride = sizeof(VertexType);

            GLint posAttrib = glGetAttribLocation(shader->shader_program_id, "Position");
            glEnableVertexAttribArray(posAttrib);
            glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, stride, 0);

            GLint colorAttrib = glGetAttribLocation(shader->shader_program_id, "VertColor");
            glEnableVertexAttribArray(colorAttrib);
            glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        }

        void set_data(polygon_<VertexType> const& verts)
        {
            LOG_IF(CheckGLError(), "Error before polygon_::set_data()");
            GL_State.bind(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(polygon_vertex_t), (void*)(points.data()), GL_STATIC_DRAW);

            set_vertex_attribs(Content.shaders["colored"]);  //TODO: either store a shader in polygon or take one as an argument

            GL_State.unbind_vao();
            GL_State.unbind_vbo();

            num_verts = points.size();

            LOG_IF(CheckGLError(), "Error during polygon creation");
            LOG("setup polygon vbo %i with %zu vertexes", vbo.id, points.size());
        }
    };

	//polygon_t polygon_rect(glm::vec2 size, const color_t& color);
	//polygon_t polygon_circle(float radius, const color_t& color, size_t num_facets = 40);
}