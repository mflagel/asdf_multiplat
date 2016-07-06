#pragma once

#include "asdfm/main/asdf_multiplat.h"
#include "asdfm/data/shader.h"
#include "asdfm/utilities/camera.h"


namespace asdf
{
namespace hexmap
{
    struct hex_map_t : asdf_specific_t
    {
        camera_t camera;
        std::shared_ptr<shader_t> shader = nullptr;

        hex_map_t();
        ~hex_map_t();

        void init();

        void update(float dt);
        void render();

        void on_event(SDL_Event*){}
    };
}
}