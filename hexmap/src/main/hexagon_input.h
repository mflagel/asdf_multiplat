#pragma once

#include <array>
#include <SDL2/SDL.h>
#include <glm/glm.h>

#include "asdf_multiplat/utilities/camera.h"

#include "data/hex_grid.h"

namespace asdf
{
namespace hexmap
{
namespace input
{

    enum mouse_button_e : uint8_t
    {
          mouse_left = 0
        , mouse_right
        , mouse_middle
        , mouse_4
        , mouse_5
    }

    struct mouse_input_t
    {
        mouse_button_e mouse_button_states;
        glm::ivec2 mouse_position;

        constexpr bool mouse_button_state(mouse_button_e btn) const
        {
            return (mouse_button_state & btn) > 0;
        }
    };

    struct sdl2_mouse_input_t : mouse_input_t
    {
        void on_event(SDL_Event* event);
    };


    struct hex_map_input_t
    {
        data::hex_grid_t& hex_grid;
        camera_t& camera;

        mouse_input_t& mouse_input;

        hex_map_input_t(data::hex_grid_t&, camera_t&);

        glm::ivec2 hex_coords_from_mouse(glm::vec2 mouse_pos)

        bool on_event(SDL_Event* event);
    };
}
}
}