#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

//#include "asdf_multiplat/utilities/camera.h"

//#include "data/hex_grid.h"

namespace asdf
{
    struct camera_t;

    namespace input
    {
        struct mouse_input_t;
    }

namespace hexmap
{
    namespace data
    {
        struct hex_grid_t;
    }
    namespace ui
    {
        struct hex_map_t;
    }

namespace input
{
    struct hex_map_input_t
    {
        ui::hex_map_t* hex_map;  /// FIXME  use weak or shared ptr, since hex_map_t is no longer the owner of this
        data::hex_grid_t& hex_grid;
        camera_t& camera;

        std::unique_ptr<asdf::input::mouse_input_t> mouse_input;

        hex_map_input_t(ui::hex_map_t*, camera_t&);
        virtual ~hex_map_input_t() = default;

        glm::vec2 world_coords() const;
        glm::ivec2 hex_coords_from_mouse(glm::ivec2 mouse_pos);

        virtual bool on_event(SDL_Event* event);
    };
}
}
}