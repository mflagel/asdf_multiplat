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

namespace ui
{
    struct hex_map_t;
}

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

    constexpr uint8_t mouse_button_bit(mouse_button_e btn)
    {
        return 1 << (uint8_t)btn;
    }

    struct mouse_input_t
    {
        uint8_t mouse_button_states = 0;
        glm::ivec2 mouse_position;

        virtual ~mouse_input_t() = default;

        bool mouse_button_state(mouse_button_e btn) const
        {
            return (mouse_button_states & mouse_button_bit(btn)) > 0;
        }

        virtual void on_event(SDL_Event* event) = 0;
    };

    struct sdl2_mouse_input_t : mouse_input_t
    {
        void on_event(SDL_Event* event);
    };

    struct hex_map_input_t
    {
        ui::hex_map_t* hex_map;
        data::hex_grid_t& hex_grid;
        camera_t& camera;

        std::unique_ptr<mouse_input_t> mouse_input;

        hex_map_input_t(ui::hex_map_t*, camera_t&);

        glm::vec2 world_coords() const;
        glm::ivec2 hex_coords_from_mouse(glm::ivec2 mouse_pos);

        bool on_event(SDL_Event* event);
    };
}
}
}