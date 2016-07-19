#include "stdafx.h"
#include "hexmap.h"

#include <glm/gtc/matrix_transform.hpp>

#include "asdf_multiplat/data/content_manager.h"
#include "asdf_multiplat/data/gl_resources.h"
#include "asdf_multiplat/utilities/spritebatch.h"


using namespace std;

using namespace glm;

namespace asdf
{
using namespace util;

namespace hexmap
{
    constexpr float max_delta_time = 1.0f; // cap dt in hex_map_t::update()

    hexmap_t::hexmap_t()
    : hex_grid(ivec2(16,16))
    {
    }

    hexmap_t::~hexmap_t()
    {

    }

    void hexmap_t::init()
    {
        ASSERT(!CheckGLError(), "GL Error before hexmap_t::init()");
        LOG("--- Initializing HexMap ---");

        app.specific = this;
        app.gl_clear_color = color_t{0.5f, 0.75f, 0.9f, 1.0f};

        auto shader = Content.create_shader("hexmap", 330);
        Content.shaders.add_resource(shader);

        camera_controller.position.x = 5; //5 hexes right
        camera_controller.position.z = 10.0; // zoom is camera.position.z ^ 2

        hex_map = make_unique<ui::hex_map_t>(hex_grid);

        ASSERT(!CheckGLError(), "GL Error in hexmap_t::init()");
    }

    void hexmap_t::update(float dt)
    {
        if(dt > max_delta_time)
        {
            dt = max_delta_time;
            LOG("**Time delta bigger than %f seconds", max_delta_time);
        }

        camera_controller.update(dt);
        camera.position = camera_controller.position;
    }

    void hexmap_t::render()
    {
        auto w = (float)app.settings.resolution_width;
        auto h = (float)app.settings.resolution_height;
        auto zoom = camera.position.z * camera.position.z;

        auto viewport = vec2(w,h) / zoom;

        auto const& shader = hex_map->shader;
        shader->use_program();
        shader->view_matrix       = camera.view_matrix();
        shader->projection_matrix = camera.projection_ortho(viewport.x / 2.0f, viewport.y / 2.0f);
        shader->update_wvp_uniform();

        hex_map->render();

        LOG_IF(CheckGLError(), "Error during hex_map_t::render()");
    }

    void hexmap_t::on_event(SDL_Event* event)
    {
        camera_controller.on_event(event);
        camera.position = camera_controller.position;
    }

}
}
