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
    : hex_grid(ivec2(15, 16))
    {
    }

    void hexmap_t::init()
    {
        ASSERT(!CheckGLError(), "GL Error before hexmap_t::init()");
        LOG("--- Initializing HexMap ---");

        app.specific = this;
        app.renderer->gl_clear_color = color_t{0.5f, 0.75f, 0.9f, 1.0f};

        auto shader = Content.create_shader("hexmap", 330);
        Content.shaders.add_resource(shader);

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

        hex_map->update(dt);
    }

    void hexmap_t::render()
    {
        hex_map->render();

        LOG_IF(CheckGLError(), "Error during hex_map_t::render()");
    }

    void hexmap_t::on_event(SDL_Event* event)
    {
        hex_map->on_event(event);
    }

}
}
