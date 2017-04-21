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
    : map_data(uvec2(0, 0))
    {
    }

    void hexmap_t::init()
    {
        ASSERT(!CheckGLError(), "GL Error before hexmap_t::init()");
        LOG("--- Initializing HexMap ---");

        app.specific = this;
        app.renderer->gl_clear_color = color_t{0.5f, 0.75f, 0.9f, 1.0f};

        rendered_map = make_unique<ui::hex_map_t>(map_data);
        resize(app.render_target_size().x, app.render_target_size().y);

        ASSERT(!CheckGLError(), "GL Error in hexmap_t::init()");
    }

    void hexmap_t::resize(uint32_t w, uint32_t h)
    {
        auto& camera = rendered_map->camera;
        camera.set_aspect_ratio(w, h);
        camera.viewport = viewport_for_size_aspect(map_data.hex_grid.size_units(), camera.aspect_ratio);
    }

    void hexmap_t::update(float dt)
    {
        if(dt > max_delta_time)
        {
            dt = max_delta_time;
            LOG("**Time delta bigger than %f seconds", max_delta_time);
        }

        rendered_map->update(dt);
    }

    void hexmap_t::render()
    {
        rendered_map->render();

        LOG_IF(CheckGLError(), "Error during hex_map_t::render()");
    }

    void hexmap_t::on_event(SDL_Event* event)
    {
        rendered_map->on_event(event);
    }

}
}
