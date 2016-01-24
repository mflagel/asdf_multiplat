#pragma once;

#include "ui_base.h"

#define BUTTON_DISABLED_COLOR  glm::vec4(0.2f)
#define BUTTON_PRESSED_COLOR   glm::vec4(0.5f)
#define BUTTON_HIGHLIGHT_COLOR glm::vec4(1.5f)

namespace asdf
{
	struct ui_button_t : ui_base_t
	{
	    enum button_state_e {
	        up,
	        down,
	        highlighted,
	        disabled
	    };

	    button_state_e state = up;
	    bool togglable; //todo: make toggle button a subclass?
	    bool toggled;

	    std::shared_ptr<ui_base_t> base_unpressed;
	    std::shared_ptr<ui_base_t> base_highlight;
	    std::shared_ptr<ui_base_t> base_pressed;
	    std::shared_ptr<ui_base_t> base_disabled;

	    ui_function_t on_start_callback;
	    ui_function_t on_end_callback;
	    ui_function_t on_end_inside_callback;

	    ui_button_t(glm::vec3 position, glm::vec3 size, bool togglable = false);

	    // void align() override;

	    virtual void update(float dt) override;
	    virtual void render(UI_RENDER_PARAMS);
	    virtual bool on_event(SDL_Event*, glm::vec3 const& _position, glm::mat3 const& _matrix) override;
	};
}
