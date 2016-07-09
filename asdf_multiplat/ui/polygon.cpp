#include "stdafx.h"
#include "polygon.h"

#include "data/content_manager.h"
#include "data/gl_resources.h"
#include "utilities/utilities_openGL.h"

using namespace glm;

namespace asdf
{
    using namespace util;
    using namespace std;

    ui_polygon_t::ui_polygon_t()
    :ui_base_t(glm::vec3(0.0f))
    {}

    ui_polygon_t::ui_polygon_t(polygon_t const& points)
    : ui_base_t(glm::vec3(0.0f))
    , num_verts(points.size())
    {
        set_data(points);
    }

    void ui_polygon_t::set_data(polygon_t const& points)
    {
        LOG_IF(CheckGLError(), "Error before polygon creation");
        GL_State.bind(vao);

        GL_State.bind(vbo);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(polygon_vertex_t), (void*)(points.data()), GL_STATIC_DRAW);

        {
            auto shader = Content.shaders["colored"];

            size_t stride = sizeof(polygon_vertex_t);

            GLint posAttrib = glGetAttribLocation(shader->shader_program_id, "Position");
            glEnableVertexAttribArray(posAttrib);
            glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, stride, 0);

            GLint colorAttrib = glGetAttribLocation(shader->shader_program_id, "VertColor");
            glEnableVertexAttribArray(colorAttrib);
            glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        }

        GL_State.unbind_vao();
        GL_State.unbind_vbo();

        num_verts = points.size();

        LOG_IF(CheckGLError(), "Error during polygon creation");
        LOG("setup polygon vbo %i with %zu vertexes", vbo.id, points.size());
    }


    // void ui_polygon_t::render(glm::vec3 const& _position, glm::mat3 const& _matrix, color_t const& _color)
    void ui_polygon_t::render()
    {
        GL_State.bind(vao);
        glDrawArrays(draw_mode, 0, num_verts);
        GL_State.unbind_vao();
    }

    /*
    polygon_t polygon_rect(const glm::vec2 size, const color_t& color)
    {
        return polygon_t{
              polygon_vertex_t{glm::vec3{-size.x, -size.y, 0.0f}, color}
            , polygon_vertex_t{glm::vec3{-size.x,  size.y, 0.0f}, color}
            , polygon_vertex_t{glm::vec3{ size.x,  size.y, 0.0f}, color}
            , polygon_vertex_t{glm::vec3{ size.x, -size.y, 0.0f}, color}
        };
    }

    polygon_t polygon_circle(float radius, const color_t& color, size_t num_facets)
    {
        polygon_t polygon;

        for(size_t i = 0; i < num_facets; ++i)
        {
            float angle = ((2.0f * PI) / num_facets) * i;
            
            glm::vec3 pos{cos(angle), sin(angle), 0.0f};  //theoretically I could pull y coords from x coords shifted pi/2
            pos *= radius;

            polygon.push_back({pos, color});
        }

        return polygon;
    }
    */
}