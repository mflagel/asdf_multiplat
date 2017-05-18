#include "terrain_brush_renderer.h"

#include "asdf_multiplat/main/asdf_defs.h"

#include "data/hex_util.h"
#include "ui/hex_grid.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace hexmap
{
namespace ui
{
    gl_vertex_spec_<vertex_attrib::position3_t> terrain_brush_vertex_t::vertex_spec;


    polygon_<terrain_brush_vertex_t> verts_for_terrain_brush(data::terrain_brush_t const& brush)
    {
        polygon_<terrain_brush_vertex_t> brush_verts;
        brush_verts.resize(brush.num_hexes() * 6);

        size_t hex_count = 0;

        for(int y = 0; y < brush.size().y; ++y)
        {
            for(int x = 0; x < brush.size().x; ++x)
            {
                if(!brush.cell_is_empty(x,y))
                {
                    auto* verts = brush_verts.data() + (hex_count * 6);

                    verts[hex_count].position.x = hexagon_points[hex_count*6 + 0];
                    verts[hex_count].position.y = hexagon_points[hex_count*6 + 1];
                    verts[hex_count].position.z = hexagon_points[hex_count*6 + 2];

                    ++hex_count;
                }
            }
        }

        return brush_verts;
    }



    void terrain_brush_renderer_t::init(std::shared_ptr<shader_t> _shader)
    {
        shader = std::move(_shader);

        brush_geometry.initialize(shader);
    }

    void terrain_brush_renderer_t::set_brush(data::terrain_brush_t* _brush)
    {
        //if pointer and contents are the same, dont bother rebuilding
        if(brush && (brush == _brush) && (*brush == *_brush))
            return;
        

        brush = _brush;

        if(brush)
            rebuild_geometry();
        else
            brush_geometry.num_verts = 0;
    }

    void terrain_brush_renderer_t::rebuild_geometry()
    {
        ASSERT(brush, "cannot rebuild geometry for a null brush");
        brush_geometry.set_data(verts_for_terrain_brush(*brush));
    }

    void terrain_brush_renderer_t::render()
    {
        ASSERT(shader, "cannot render terrain brush without a shader");

        //if after rebuilding, there is nothing to render, don't bother with anything below
        if(brush_geometry.num_verts == 0)
        {
            return;
        }

        GL_State->bind(shader);
        shader->update_wvp_uniform();

        brush_geometry.render(GL_TRIANGLE_STRIP);

        ASSERT(!CheckGLError(), "Error in terrain_brush_renderer_t::render()");
    }
}
}
}