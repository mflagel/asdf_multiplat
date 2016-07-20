#pragma once

#include <array>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

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
    };

    struct mouse_input_t
    {
        uint8_t mouse_button_states = 0;
        glm::ivec2 mouse_position;

        virtual ~mouse_input_t(){}

        bool mouse_button_state(mouse_button_e btn) const
        {
            return (mouse_button_states & (uint8_t)btn) > 0;
        }

        virtual void on_event(SDL_Event* event) = 0;
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

        glm::ivec2 hex_coords_from_mouse(glm::vec2 mouse_pos);

        bool on_event(SDL_Event* event);
    };
}
}
}