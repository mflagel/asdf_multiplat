#include "stdafx.h"
#include "gurps_track.h"

#include "ui_wrapper.h"
#include "character.h"
#include "character_layout.h"
#include "skill_list_layout.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace gurps_track
{
    gurps_track_t::gurps_track_t()
    {
    }

    gurps_track_t::~gurps_track_t()
    {

    }

    void gurps_track_t::init()
    {
        LOG("--- Initializing Gurps Track ---");

        fps_label = app.main_view->add_child<ui_label_t>("fps: 9001", Content.fonts["arial"]);
        fps_label->alignment = ui_align_top_left;

// #ifdef DEBUG
        // debug_btn = app.main_view->add_child<button_view_t>("debug", 
        //     [this](ui_base_t*)
        //     {
        //         app.render_debug_views = debug_btn->btn->toggled;
        //     });
        // debug_btn->btn->togglable = true;
        // debug_btn->alignment = ui_align_top_right;
        // debug_btn->set_size(vec2(100, 50));
// #endif

        character = make_shared<character_t>(character_filepath);
        auto char_layout = app.main_view->add_child<character_layout_t>(character);
        char_layout->alignment = ui_align_top;
        char_layout->offset = vec3(-220, -100, 0.0f);

        auto skill_lib_view = app.main_view->add_child<skill_library_t>(character);
        skill_lib_view->alignment = ui_align_top;
        skill_lib_view->set_size(vec2(400.0f, 800.0f));
        skill_lib_view->offset = vec3(220, -100, 0.0f);

        app.main_view->align();
    }

    void gurps_track_t::process_args(int argc, char* argv[])
    {
        for(int i = 1; i < argc; ++i)
        {
            process_arg(argv[i]);
        }
    }

    void gurps_track_t::process_arg(char* arg)
    {
        if(arg[0] == '-')
        {
            //todo: process flag
        }
        else
        {
            character_filepath = std::string(arg);
        }
    }

    void gurps_track_t::update(float dt)
    {
        app.main_view->position = -camera.position;

        fps_label->set_text("fps: " + to_string(1.0f / app.average_frame_time()));
    }

    void gurps_track_t::render()
    {
        
    }

    void gurps_track_t::on_event(SDL_Event* event)
    {
        camera.on_event(event);
    }
}
}