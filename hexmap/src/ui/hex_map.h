#pragma once

#include <glm/glm.hpp>


#include <asdfm/main/input_controller.h>
#include <asdfm/ui/ui_base.h>
#include <asdfm/data/texture_atlas.h>
#include <asdfm/utilities/spritebatch.h>

#include "data/hex_map.h"
#include "data/terrain_bank.h"
#include "ui/hex_grid.h"
#include "ui/spline_renderer.h"

//for whatever reason Qt requires this, but libhexmap.mk does not somehow
#include <asdfm/main/input_controller.h>
#include <asdfm/ui/polygon.h>
#include <asdfm/utilities/camera.h>

using color_t = glm::vec4;

struct cJSON;

namespace asdf
{
namespace hexmap
{
namespace ui
{
    struct hex_map_t : hex_grid_t
    {
        data::hex_map_t& map_data;

        asdf::camera_t camera;
        asdf::input_handler_sdl2_t camera_controller;

        spline_renderer_t spline_renderer;
        
        spritebatch_t spritebatch; //used to render map objects

    public:
        hex_map_t(data::hex_map_t& _map_data);
        
        void update(float dt);
        void render(render_flags_e = everything);
        void on_event(SDL_Event* event);
        
        void render_map_objects();
        void render_splines() const;
    };

}
}
}
