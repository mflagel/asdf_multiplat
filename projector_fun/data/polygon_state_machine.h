#pragma once;

#include "polygon.h"

namespace asdf
{
namespace projector_fun
{
	struct polygon_state_machine_t;
    struct polygon_state_t;

    struct polygon_event_t
    {
        polygon_state_machine_t* state_machine = nullptr;

        std::shared_ptr<polygon_state_t> old_state;
        std::shared_ptr<polygon_state_t> new_state;

        polygon_event_t(std::shared_ptr<polygon_state_t> _old, std::shared_ptr<polygon_state_t> _new);
    };

	struct polygon_state_t
	{
        polygon_state_machine_t* state_machine = nullptr;

		virtual ~polygon_state_t() = default;

        virtual void update(float dt) = 0;
	};

	struct polygon_state_machine_t
	{
		std::shared_ptr<polygon_state_t> current_state = nullptr;

		void update(float dt);

        bool handle_event(std::shared_ptr<polygon_event_t>); // taking solid shared_ptr instead of a non-owning ptr or ref in case the handling of the event involves the deletion of the state/object that owns the event's memory. Beats having to sidestep that pitfall manually
		void transition_to(std::shared_ptr<polygon_state_t> new_state); //todo: forward template args to make_shared in this
	};
	
}
}