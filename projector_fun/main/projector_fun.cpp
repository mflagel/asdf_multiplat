#include "stdafx.h"
#include "projector_fun.h"

#include <glm/gtc/matrix_transform.hpp>

//#include <boost/filesystem.hpp>

#include "thread_pool.h"

#include "polygon_path.h"

using namespace std;
//using namespace boost::filesystem;

using namespace glm;

namespace asdf
{
using namespace util;

namespace projector_fun
{
    constexpr float max_delta_time = 1.0f; // cap dt in projector_fun_t::update()

    projector_fun_t::projector_fun_t()
    {
    }

    projector_fun_t::~projector_fun_t()
    {

    }

    void projector_fun_t::init()
    {
        ASSERT(!CheckGLError(), "GL Error before projector_fun_t::init()");
        LOG("--- Initializing Projector Fun ---");

        app.specific = this;
        app.gl_clear_color = COLOR_BLACK;

        shader = Content.shaders["colored"];

        shader->view_matrix       = camera.view_matrix();
        shader->projection_matrix = camera.projection_ortho(app.settings.resolution_width / 2.0f
                                                          , app.settings.resolution_height / 2.0f);

        {
            // test_polygon.set_data(polygon_circle(50.0f, COLOR_WHITE));
            test_polygon = {polygon_circle(50.0f, COLOR_WHITE)};
            // test_polygon = {polygon_rect(glm::vec2{200.0f, 200.0f}, COLOR_WHITE)};
            test_polygon.draw_mode = GL_TRIANGLE_FAN;
        }

        {  // linear delta path test
            auto path_deltas = {
                  glm::vec3( 200.0f,  200.0f, 0.0f)
                , glm::vec3(   0.0f, -400.0f, 0.0f)
                , glm::vec3(-400.0f,    0.0f, 0.0f)
            };

            polygon_path_t path = polygon_path_from_deltas(test_polygon.position, std::move(path_deltas), true);

            test_bezier_path = bezier_path_from_path(path, false);

            //test a single bezier hull
            // test_bezier_path = {
            //     bezier_path_node_t{vec3{-100, 0.0f, 0.0f}, vec3{  0.0f,   0.0f, 0.0f}, vec3{-100.0f, 100.0f, 0.0f}}
            //   , bezier_path_node_t{vec3{ 100, 0.0f, 0.0f}, vec3{100.0f, 100.0f, 0.0f}, vec3{   0.0f,   0.0f, 0.0f}, }
            // };

            auto dbg_bezier_path = generate_bezier_path(test_bezier_path, 20, true);
            bezier_path_polygon = {dbg_bezier_path};
            bezier_path_polygon.draw_mode = GL_LINES;

            // test_machine.current_state = make_shared<polygon_path_state_t>(test_polygon, path);
            test_machine.current_state = make_shared<polygon_path_state_t>(test_polygon, test_bezier_path);
        }

        {
            // auto metaballs_shader = make_shared<shader_t>("metaball", "../../shaders/passthrough_120.vert", "../../shaders/metaballs_120.frag");
            // Content.shaders.add_resource("metaball", std::move(metaballs_shader));
        }

        {
            std::array<int,500000> test_array;

            // divided_task_t test_div(test_array)
        }

        ASSERT(!CheckGLError(), "GL Error in projector_fun_t::init()");
    }

    void projector_fun_t::update(float dt)
    {
        if(dt > max_delta_time)
        {
            dt = max_delta_time;
            LOG("**Time delta bigger than %f seconds", max_delta_time);
        }

        test_machine.update(dt);

        metaballs.update(dt);
    }

    void projector_fun_t::render()
    {
        shader->use_program();

        render_test_polygon();
        render_bezier_curves();
        // render_metaballs();

        LOG_IF(CheckGLError(), "Error during projector_fun_t::render()");
    }

    void projector_fun_t::render_test_polygon()
    {
        shader->world_matrix = glm::translate(shader->world_matrix, test_polygon.position);
        shader->update_wvp_uniform();
        test_polygon.render();
    }

    void projector_fun_t::render_bezier_curves()
    {
        shader->world_matrix = glm::mat4();
        shader->update_wvp_uniform();

        glBindVertexArray(bezier_path_polygon.vao);
        glLineWidth(1.0f);
        glDrawArrays(GL_LINES, 0, bezier_path_polygon.num_verts);
        glBindVertexArray(0);
    }

    void projector_fun_t::render_metaballs()
    {
        metaballs.draw_metaballs(); //writes data to metaballs_t::texture

        app.spritebatch->begin();
        app.spritebatch->draw(metaballs.texture, vec2{0.0f});
        // app.spritebatch->draw(Content.textures["debug"], vec2{0.0f});
        app.spritebatch->end();
    }

}
}
