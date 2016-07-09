#pragma clang diagnostic ignored "-Wunused-parameter"

#include "stdafx.h"
#include <asdf_multiplat/main/asdf_multiplat.h>
#include <asdf_multiplat/utilities/utilities.h>

#include "hexmap.h"

using namespace asdf;
using namespace hexmap;

int main(int argc, char* argv[]) 
{
    //todo: make sure argv is actually the full exec path in UNIX
    std::string exec_dir(argv[0]);
    size_t new_size = exec_dir.length();
    for (;exec_dir[new_size] != '\\' && exec_dir[new_size] != '/' && --new_size != 0;);
    exec_dir.resize(new_size);

    SetCurrentDir(exec_dir.c_str());

    app.init(std::move(exec_dir));

    hexmap_t hexmap;
    app.specific = &hexmap;
    hexmap.init();

	SDL_Event event;

    while (app.running) {
        while (SDL_PollEvent(&event)) {
            app.on_event(&event);
        }

        app.update();
        app.render();
    }

    LOG("--- Another Successful Procedure ---");

	return 0;
}