#include "stdafx.h"
#include "ui_button.h"
#include "utilities.h"

using namespace glm;
using namespace std;
using namespace asdf::util;

namespace asdf
{
	ui_button_t::ui_button_t(vec3 _position, vec3 size, bool togglable)
	    : ui_base_t(_position)
	    , state(up)
	    , togglable(togglable)
	    , toggled(false)
	{
	    upper_bound = position + size / 2.0f;
	    lower_bound = position - size / 2.0f;
	}

	void ui_button_t::update(float dt)
	{
	    ui_base_t::update(dt);
	}

	void ui_button_t::render(vec3 const& _position, mat3 const& _matrix, color_t const& _color)
	{
	    DEFAULT_TRANSFORM_COPYPASTA

	    base_highlight->visible = state == highlighted;
	    base_pressed->  visible = state == down || (toggled && state == up);
	    base_disabled-> visible = state == disabled;

	    base_unpressed->visible = !base_highlight->visible
	                           && !base_pressed->visible
	                           && !base_disabled->visible;

	    //recolor base_pressed for other button states if necessary
	    if(state == down && !base_pressed)
	        base_unpressed->render(position_, matrix_, color_ * BUTTON_PRESSED_COLOR);

	    else if(state == highlighted && !base_highlight)
	        base_unpressed->render(position_, matrix_, color_ * BUTTON_HIGHLIGHT_COLOR);

	    else if(state == disabled && !base_disabled)
	        base_unpressed->render(position_, matrix_, color_ * BUTTON_DISABLED_COLOR);

	    ui_base_t::render(_position, _matrix, _color);

	    base_unpressed->visible = state == up;
	}

	// #define BUTTON_LOG() {}
	#define BUTTON_LOG(event_type) LOG("%s %s  xy:(%f,%f)  upper:(%f,%f) lower(%f,%f)"              \
													, #event_type, inside ? "inside" : "not-inside" \
													, mouse_abspos.x, mouse_abspos.y                \
													, upper_bound.x, upper_bound.y                  \
													, lower_bound.x, lower_bound.y);
	//----

	bool ui_button_t::on_event(SDL_Event* event , glm::vec3 const& _position, glm::mat3 const& _matrix)
	{
	    auto pos = position + _position;

	    vec3 mouse_abspos(event->motion.x, event->motion.y, 0.0f);
	    vec3 mouse_relpos = mouse_abspos - pos;

	    bool inside = contains_point(mouse_relpos);

	    switch (event->type)
	    {
	        case SDL_MOUSEMOTION:
	        {
	            if(inside)
	            {
	                state = (state == down) ? down : highlighted;
	            }
	            else
	            {
	                state = up;
	            }
	            break;
	        };

	        case SDL_MOUSEBUTTONDOWN:
	        {
	            if (inside)
	            {
	            	BUTTON_LOG(mouse_down);
	                // state = toggled ? highlighted : down;
	                state = down;

	                if (on_start_callback)
	                {
	                    on_start_callback(this);
	                    return true;
	                }
	            }
	            break;
	        }
	        case SDL_MOUSEBUTTONUP:
	        {
	            if (inside)
	            {
	            	BUTTON_LOG(mouse_up);
	            	
	                toggled = !toggled && togglable;
	                // state = toggled ? down : highlighted;
	                state = highlighted;

	                if (on_end_inside_callback)
	                {
	                    on_end_inside_callback(this);
	                    return true;
	                }
	            } else {
	                // state = toggled ? state : up;
	                state = up;

	                if (on_end_callback)
	                {
	                    on_end_callback(this);
	                    return true;
	                }
	            }
	            break;
	        }
	    }

	    return ui_base_t::on_event(event, _position, _matrix);
	}
}