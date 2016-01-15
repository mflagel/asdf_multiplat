#pragma once

#include <SDL\SDL_mouse.h>

namespace asdf {

    struct mouse_state_t {
        enum mouse_button_e : unsigned short {
            mouse_button_none       = 0,
            mouse_button_left       = 1,
            mouse_button_middle     = 2,
            mouse_button_right      = 4,
            mouse_button_4          = 8,
            mouse_button_5          = 16,
        };

        float x, y, z;
        float prevx, prevy, prevz;
        
        unsigned short button_state;
        unsigned short prev_button_state;

        mouse_state_t()
            : x(0.0f), y(0.0f), z(0.0f)
            , prevx(0.0f), prevy(0.0f), prevz(0.0f)
            , button_state(mouse_button_none)
            , prev_button_state(mouse_button_none) 
        {}

        inline void update() {            
            prevx = x; prevy = y; prevz = z;
            prev_button_state = button_state;

            int ix, iy;
            button_state = SDL_GetRelativeMouseState(&ix, &iy);
            x = float(ix); y = float(iy);
        }
    };

}