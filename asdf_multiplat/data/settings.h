#pragma once;

namespace asdf {

    struct settings_t {

        size_t resolution_width = 1280;
        size_t resolution_height = 720;

        bool fullscreen = false;
        bool borderless = false;


        float mouse_sensitivity;
    };
}