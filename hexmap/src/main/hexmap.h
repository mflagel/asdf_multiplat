#pragma once

#include "asdf_multiplat/main/asdf_multiplat.h"
#include "asdf_multiplat/data/shader.h"
#include "asdf_multiplat/utilities/camera.h"
#include "asdf_multiplat/utilities/utilities.h"

#include "ui/hex_map.h"

namespace asdf
{

namespace hexmap
{
    struct hexmap_t : asdf_specific_t
    {
        camera_t camera;

        std::unique_ptr<ui::hex_map_t> test_hex_map;

        hexmap_t();
        ~hexmap_t();

        void init();

        void update(float dt);
        void render();

        void on_event(SDL_Event*);
    };
}
}