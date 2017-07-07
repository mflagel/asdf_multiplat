#pragma once

namespace asdf {

    struct settings_t {

        uint32_t resolution_width = 1280;
        uint32_t resolution_height = 720;

        bool fullscreen = false;
        bool borderless = false;
        bool resizable = true;


        float mouse_sensitivity;
    };
}