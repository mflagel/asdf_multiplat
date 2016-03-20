#include "stdafx.h"
#include "character_skill_list.h"

#include "data/character.h"

using namespace std;
using namespace mk;

namespace gurps_track
{

using namespace data;

namespace ui
{


	character_skill_list_entry_t::character_skill_list_entry_t(character_skill_list_t& _parent, size_t _index)
	: Expandbox(" ", true)
	, description(emplace<Label>("dsc"))
	, parent(_parent)
	, index(_index)
	{
		auto improve_skill = [this](Button&)
		{
			parent.character.improve_skill(index);
		};
		auto un_improve_skill = [this](Button&)
		{
			/// TODO: un_improve button callback
		};

		name            = &mHeader->emplace<Label>("n");
		difficulty      = &mHeader->emplace<Label>("d");
		point_cost      = &mHeader->emplace<Label>("[-]");
		effective_skill = &mHeader->emplace<Label>("-");
		improve_btn     = &mHeader->emplace<Button>("+", std::move(improve_skill));
		un_improve_btn  = &mHeader->emplace<Button>("-", std::move(un_improve_skill));

		set_data(parent.character, index);
		collapse();
	}

	void character_skill_list_entry_t::set_data(data::character_t const& character, size_t skill_index)
	{
		learned_skill_t const& learned_skill = character.skills[skill_index];
		auto const& skill = learned_skill.skill;
		name->setLabel(skill.name);
		difficulty->setLabel(  "(" + std::string(skill_difficulty_abbreviations[skill.difficulty]) + ")" );

		point_cost->setLabel( "[" + to_string(learned_skill.point_cost()) + "]  " );
		effective_skill->setLabel(to_string(learned_skill.get_effective_skill(character)));

		// description.setLabel(skill.description);
		description.setLabel("test description");
	}

	character_skill_list_t::character_skill_list_t(data::character_t& _character)
	: Board()
	, character(_character)
	{
		//todo: make the tab rebuild automatically if the number of learned
		//      skills changes
		emplace<Button>("Refresh", [this](Button&){ rebuild(); });

		scroll_list = &emplace<List>();

		rebuild();
	}

	void character_skill_list_t::rebuild()
	{
		scroll_list->clear();
		for(size_t i = 0; i < character.skills.size(); ++i)
		{
			scroll_list->emplace<character_skill_list_entry_t>(*this, i);
		}
	}

}
}