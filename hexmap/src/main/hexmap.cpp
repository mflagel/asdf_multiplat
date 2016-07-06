#include "stdafx.h"
#include "hexmap.h"

#include <glm/gtc/matrix_transform.hpp>

#include "asdfm/data/content_manager.h"
#include "asdfm/utilities/spritebatch.h"
#include "asdfm/utilities/utilities_openGL.h"


using namespace std;

using namespace glm;

namespace asdf
{
using namespace util;

namespace hexmap
{
    constexpr float max_delta_time = 1.0f; // cap dt in hex_map_t::update()

    hex_map_t::hex_map_t()
    {
    }

    hex_map_t::~hex_map_t()
    {

    }


    // TODO: build a decent asset loading system
    // this is temporary
    constexpr char hexmap_root_dir[] = "../../..";
    constexpr char assets_dir[] = "../../../../assets";
    constexpr char shaders_dir[] = "../../../../shaders/330";

    void hex_map_t::init()
    {
        ASSERT(!CheckGLError(), "GL Error before projector_fun_t::init()");
        LOG("--- Initializing Projector Fun ---");

        app.specific = this;
        app.gl_clear_color = color_t{0.5f, 0.75f, 0.9f, 1.0f};

        Content.AddSamplers(); //doing this here because I don't init Content to avoid it failing to load a bunch of assets because the executable is in a different location than expected

        //shader = Content.shaders["colored"];

        //TODO: change shader to take strings instead of const char*
        shader = make_unique<shader_t>("test", (string(shaders_dir) + string("/passthrough_330.vert")).c_str(), (string(shaders_dir) + string("/passthrough_330.frag")).c_str());

        shader->view_matrix       = camera.view_matrix();
        shader->projection_matrix = camera.projection_ortho(app.settings.resolution_width / 2.0f
                                                          , app.settings.resolution_height / 2.0f);


        //hack
        app.spritebatch->spritebatch_shader = make_unique<shader_t>("spritebatch", (string(shaders_dir) + string("/spritebatch_330.vert")).c_str(), (string(shaders_dir) + string("/spritebatch_330.frag")).c_str());


        ASSERT(!CheckGLError(), "GL Error in projector_fun_t::init()");
    }

    void hex_map_t::update(float dt)
    {
        if(dt > max_delta_time)
        {
            dt = max_delta_time;
            LOG("**Time delta bigger than %f seconds", max_delta_time);
        }
    }

    void hex_map_t::render()
    {
        shader->use_program();



        LOG_IF(CheckGLError(), "Error during hex_map_t::render()");
    }

}
}
