#pragma once

#include <kiui/Ui/mkUiConfig.h>
#include <kiui/Ui/mkUiTypes.h>

#include "data/skills.h"

namespace gurps_track
{

namespace data
{
	struct character_t;
}

namespace ui
{

	struct spell_list_t;

	/*
	+---------------------+
	|-Spell Name - 15 |+|-|
	| Cost: 2   Maint: 1  |
	| TTC:  1   Dur: 1m   |
	|- Description       -|
	|   Discription is    |
	|   collapsable to not|
	|   clog up vertical  |
	|   space when showing|
	|   cost and such     |
	+---------------------+
	*/
	struct spell_list_entry_t : mk::Expandbox
	{
		mk::Label* name        = nullptr;
		mk::Label* difficulty  = nullptr;
		mk::InputBool* add_remove_button = nullptr;

		mk::Label* cost;
		mk::Label* maintenance;
		mk::Label* time_to_cast;
		mk::Label* duration;
		mk::Label* description;

		spell_list_t& parent;
		size_t index;

		spell_list_entry_t(spell_list_t& _parent, size_t index);

		void set_data(data::spell_t const&);
	};

	/*
	+---------------------+
	|-|+|Spell Name      -|
	|  details            |
	|-|+|SkillA          -|
	|-|+|SkillZZ         -|
	+---------------------+
	*/
	struct spell_list_t : mk::Board
	{
		data::spell_list_t const& spells;
		data::character_t* character = nullptr;

		spell_list_t(data::spell_list_t const&, data::character_t* _character = nullptr);
	};
}
}