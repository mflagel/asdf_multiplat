#pragma once

#include "gl_resources.h"

namespace asdf
{
namespace projector_fun
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

	    ui_polygon_t operator=(const ui_polygon_t&);

	    void set_data(polygon_t const& points);

	    // void render(glm::vec3 const& _position, glm::mat3 const& _matrix, color_t const& _color);
	    void render();
	};

	polygon_t polygon_rect(glm::vec2 size, const color_t& color);
	polygon_t polygon_circle(float radius, const color_t& color, size_t num_facets = 40);
}
}