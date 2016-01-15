#include "stdafx.h"
#include "polygon_state_machine.h"

using namespace std;

namespace asdf
{
namespace projector_fun
{
	polygon_event_t::polygon_event_t(shared_ptr<polygon_state_t> _old, shared_ptr<polygon_state_t> _new)
	: old_state(_old)
	, new_state(_new)
	{
	    ASSERT(_old->state_machine, "transitioning from an orphaned state");
	    state_machine = _old->state_machine;
	}

	void polygon_state_machine_t::update(float dt)
	{
		ASSERT(current_state, "Why are you running a state machine without a state");
		current_state->update(dt);
	}

	bool polygon_state_machine_t::handle_event(std::shared_ptr<polygon_event_t> event)
	{
		return false;
	}

	void polygon_state_machine_t::transition_to(std::shared_ptr<polygon_state_t> new_state)
	{
		// todo: delayed transitions. pretty sure I can do this with lambdas
		// ex: new state does not become current state until old/current state is done transitioning
		// could be that the transition is just another intermediary state
		// current_state->transitioned_from(new_state);

		current_state = new_state;
		current_state->state_machine = this;
	}

}
}