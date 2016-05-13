#include "stdafx.h"
#include "spell_list.h"

#include "data/character.h"

using namespace std;
using namespace mk;

namespace gurps_track
{

using namespace data;

namespace ui
{


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
	spell_list_entry_t::spell_list_entry_t(spell_list_t& _parent, size_t _index)
	: Expandbox(" ", true)
	, parent(_parent)
	, index(_index)
	{
		auto button_toggle_func = [this](bool on)
		{
			if(!on)
			{
				ASSERT(parent.character, "This button should not be pressable if the spell list does not have an attached character");
				parent.character->learn_spell(parent.spells[index], 1);
			}
			else
			{
				//todo
			}
		};

		name       = &mHeader->emplace<Label>("n");
		difficulty = &mHeader->emplace<Label>("n");
		add_remove_button = &mHeader->emplace<InputBool>("<-", true, std::move(button_toggle_func), true);

		Sequence& seq1 = emplace<Sequence>();
		seq1.emplace<Label>("Cost: ");
		cost  		= &seq1.emplace<Label>("c");
		seq1.emplace<Label>("Maint: ");
		maintenance = &seq1.emplace<Label>("m");

		Sequence& seq2 = emplace<Sequence>();
		seq2.emplace<Label>("TTC: ");
		time_to_cast = &seq2.emplace<Label>("t");
		seq2.emplace<Label>("Duration: ");
		duration     = &seq2.emplace<Label>("d");

		Expandbox& desc_box = emplace<Expandbox>("Description");
		description = &desc_box.emplace<Label>("-");

		set_data(parent.spells[index]);
		collapse();
	}

	void spell_list_entry_t::set_data(spell_t const& spell)
	{
		name->setLabel(spell.name);

		if(spell.difficulty != skill_difficulty_hard)
		{
			difficulty->setLabel(  "(" + std::string(skill_difficulty_abbreviations[spell.difficulty]) + ")" );
		}
		else
		{
			difficulty->setLabel(" ");
		}


		cost->        setLabel(spell.initial_cost);
		maintenance-> setLabel(spell.maintenance_cost);
		time_to_cast->setLabel(spell.time_to_cast);
		duration->    setLabel(spell.duration);

		description->setLabel(spell.description);

		if(parent.character)
		{
			add_remove_button->show();
			bool can_learn = parent.character->has_spell(spell);
			add_remove_button->input().value()->set<bool>(can_learn);
		}
		else
		{
			add_remove_button->hide();
		}
	}

	spell_list_t::spell_list_t(data::spell_list_t const& _spells, data::character_t* _character)
	: spells(_spells)
	, character(_character)
	{
		List& list = emplace<List>();

		for(size_t i = 0; i < spells.size(); ++i)
		{
			list.emplace<spell_list_entry_t>(*this, i);
		}
	}

}
}