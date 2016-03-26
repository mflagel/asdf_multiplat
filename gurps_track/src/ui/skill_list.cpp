#include "stdafx.h"
#include "skill_list.h"

#include "data/character.h"

using namespace std;
using namespace mk;

namespace gurps_track
{

using namespace data;

namespace ui
{


	skill_list_entry_t::skill_list_entry_t(skill_list_t& _parent, size_t _index)
	: Expandbox(" ", true)
	, description(emplace<Label>("dsc"))
	, parent(_parent)
	, index(_index)
	{
		auto button_toggle_func = [this](bool on)
		{
			if(!on)
			{
				ASSERT(parent.character, "This button should not be pressable if the skill list does not have an attached character");
				parent.character->learn_skill(parent.skills[index], 1);
			}
			else
			{
				//todo
			}
		};

		name       = &mHeader->emplace<Label>("n");
		difficulty = &mHeader->emplace<Label>("n");
		add_remove_button = &mHeader->emplace<InputBool>("<-", true, std::move(button_toggle_func), true);

		set_data(parent.skills[index]);
		collapse();
	}

	void skill_list_entry_t::set_data(skill_t const& skill)
	{
		name->setLabel(skill.name);
		difficulty->setLabel(  "(" + skill.difficulty_string() + ")" );

		description.setLabel(skill.description);
		description.setLabel("test description");

		if(parent.character)
		{
			add_remove_button->show();
			bool can_learn = parent.character->has_skill(skill);
			add_remove_button->input().value()->set<bool>(can_learn);
		}
		else
		{
			add_remove_button->hide();
		}
	}

	skill_list_t::skill_list_t(data::skill_list_t const& _skills, data::character_t* _character)
	: Board()
	, skills(_skills)
	, character(_character)
	{
		List& list = emplace<List>();

		for(size_t i = 0; i < skills.size(); ++i)
		{
			list.emplace<skill_list_entry_t>(*this, i);
		}
	}

}
}