#include "stdafx.h"
#include "asdf_multiplat.h"

DIAGNOSTIC_IGNORE(-Wunused-parameter)

using namespace asdf;

int main(int argc, char* argv[]) 
{
	LOG("\n--- asdf_multiplat ---");

	app.init("todo: this");

	SDL_Event event;
 	while (app.running) {
        while (SDL_PollEvent(&event)) {
            app.on_event(&event);
        }
        app.update();
        app.render();
     }

    LOG("--- Another Successful Procedure ---\n");
	return 0;
}
