#pragma once

#include "asdf_multiplat.h"
#include "camera.h"

#include "character.h"

namespace asdf
{

    struct ui_label_t;
    struct button_view_t;

namespace gurps_track
{

    struct gurps_track_t : asdf_specific_t
    {
        camera_t camera;
        ui_label_t* fps_label = nullptr;
        button_view_t* debug_btn = nullptr;

        std::string character_filepath = "";
        std::shared_ptr<character_t> character;

        gurps_track_t();
        ~gurps_track_t();

        void init();
        void process_args(int argc, char* argv[]);
        void process_arg(char*);

        void update(float dt);
        void render();

        void on_event(SDL_Event*);
    };

}
}