#include "stdafx.h"
#include "hexmap.h"

#include <glm/gtc/matrix_transform.hpp>

#include "asdf_multiplat/data/content_manager.h"
#include "asdf_multiplat/data/gl_resources.h"
#include "asdf_multiplat/utilities/spritebatch.h"
#include "asdf_multiplat/utilities/utilities_openGL.h"


using namespace std;

using namespace glm;

namespace asdf
{
using namespace util;

namespace hexmap
{
    constexpr float max_delta_time = 1.0f; // cap dt in hex_map_t::update()

    hexmap_t::hexmap_t()
    {
    }

    hexmap_t::~hexmap_t()
    {

    }


    struct test_polygon_vert_t
    {
        static gl_vertex_spec_<vertex_attrib::position3_t, vertex_attrib::color_t> vertex_spec;

        glm::vec3 test_pos;
        glm::vec4 test_color;
    };

    gl_vertex_spec_<vertex_attrib::position3_t, vertex_attrib::color_t> test_polygon_vert_t::vertex_spec;

    constexpr char VertexPosition[] = "VertexPosition";

    void hexmap_t::init()
    {
        ASSERT(!CheckGLError(), "GL Error before projector_fun_t::init()");
        LOG("--- Initializing Projector Fun ---");

        app.specific = this;
        app.gl_clear_color = color_t{0.5f, 0.75f, 0.9f, 1.0f};

        shader = Content.shaders["colored"];
        ASSERT(!CheckGLError(), "why does this fail");

        Content.shaders.add_resource( "colored",  shader);
        shader->use_program();

        shader->view_matrix       = camera.view_matrix();
        shader->projection_matrix = camera.projection_ortho(app.settings.resolution_width  / 2.0f
                                                          , app.settings.resolution_height / 2.0f);

        shader->update_wvp_uniform();


        //hack
        //app.spritebatch->spritebatch_shader = make_unique<shader_t>("spritebatch", (string(shaders_dir) + string("/spritebatch_330.vert")).c_str(), (string(shaders_dir) + string("/spritebatch_330.frag")).c_str());

        test_hex_map = make_unique<ui::hex_map_t>();


        /// TEST
        test_polygon_vert_t test_vert;
        LOG("test size  %zu", sizeof(test_vert));

        auto test = test_vert.vertex_spec;
        //LOG("test stride  %zu", .get_stride());
        

        ASSERT(!CheckGLError(), "GL Error in projector_fun_t::init()");
    }

    void hexmap_t::update(float dt)
    {
        if(dt > max_delta_time)
        {
            dt = max_delta_time;
            LOG("**Time delta bigger than %f seconds", max_delta_time);
        }
    }

    void hexmap_t::render()
    {
        shader->use_program();

        test_hex_map->render_grid_overlay();

        LOG_IF(CheckGLError(), "Error during hex_map_t::render()");
    }

}
}
