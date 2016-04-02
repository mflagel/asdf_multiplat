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

	struct spell_properties_t;

namespace character
{
	struct spell_list_t;

	/*
	+--------------------------+
	|-Spell Name - (1) 14 [+|-]|
	| Cost: 2   Maint: 1       |
	| TTC:  1   Dur: 1m        |
	|- Description       -     |
	|   Discription is         |
	|   collapsable to not     |
	|   clog up vertical       |
	|   space when showing     |
	|   cost and such          |
	+--------------------------+
	 */
	struct spell_list_entry_t : mk::Expandbox
	{
		mk::Label*  name            = nullptr;
		mk::Label*  difficulty      = nullptr;
		mk::Label*  point_cost      = nullptr;
		mk::Label*  effective_skill = nullptr;
		mk::Button* improve_btn     = nullptr;
		mk::Button* un_improve_btn  = nullptr;

		spell_properties_t* properties = nullptr;

		mk::Label* description = nullptr;

		spell_list_t& parent;
		size_t index = 900001;

		spell_list_entry_t(spell_list_t& _parent, size_t index);

		void set_data(data::character_t const&, size_t spell_index);
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
		mk::List* list;
		data::character_t& character;

		spell_list_t(data::character_t& _character);

		void rebuild();
	};

}
}
}