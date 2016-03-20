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
		improve_btn     = &mHeader->emplace<Button>("+");
		un_improve_btn  = &mHeader->emplace<Button>("-");

		set_data(parent.character.skills[index]);
		collapse();
	}

	void character_skill_list_entry_t::set_data(learned_skill_t const& learned_skill)
	{
		auto const& skill = learned_skill.skill;
		name->setLabel(skill.name);
		difficulty->setLabel(  "(" + std::string(skill_difficulty_abbreviations[skill.difficulty]) + ")" );

		description.setLabel(skill.description);
		description.setLabel("test description");
	}

	character_skill_list_t::character_skill_list_t(data::character_t& _character)
	: Board()
	, character(_character)
	{
		List& list = emplace<List>();

		for(size_t i = 0; i < character.skills.size(); ++i)
		{
			// list.emplace<skill_library_entry_t>(*this, i);
		}
	}

}
}