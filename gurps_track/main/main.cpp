#pragma clang diagnostic ignored "-Wunused-parameter"

#include "stdafx.h"
#include "asdf_multiplat.h"
#include "gurps_track.h"
#include "utilities.h"

using namespace asdf;
using namespace gurps_track;

namespace
{
    constexpr float frame_time = 1.0f / 15.0f;
    constexpr uint32 frame_delay_ms = static_cast<uint32>(frame_time * 1000);
}

int main(int argc, char* argv[]) 
{
    //todo: make sure argv is actually the full exec path in UNIX
    std::string exec_dir(argv[0]);
    size_t new_size = exec_dir.length();
    for (;exec_dir[new_size] != '\\' && exec_dir[new_size] != '/' && --new_size != 0;);
    exec_dir.resize(new_size);

    SetCurrentDir(exec_dir.c_str());

    app.init(std::move(exec_dir));

    gurps_track_t gurpstrack;
    app.specific = &gurpstrack;
    gurpstrack.process_args(argc, argv);
    gurpstrack.init();

	SDL_Event event;

    while (app.running) {
        while (SDL_PollEvent(&event)) {
            app.on_event(&event);
        }

        SDL_Delay(frame_delay_ms);

        app.update();
        app.render();
    }

    LOG("--- Another Successful Procedure ---");

	return 0;
}