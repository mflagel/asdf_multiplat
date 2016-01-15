#pragma once

#include "stdafx.h"
#include <SDL/SDL_events.h>

#include "ui_base.h"

namespace asdf
{
	struct drag_control_t
	{
		std::shared_ptr<ui_base_t> base;

		bool is_dragging;

		void on_event(SDL_Event*);
	};

	namespace
	{
		float drag_edge_dist_px = 10; //how many px away from a drage edge before dragging is available
	}
}