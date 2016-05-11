#include "stdafx.h"
#include "spell_list.h"

#include "data/character.h"
#include "ui/spell_list.h"

using namespace std;
using namespace mk;

namespace gurps_track
{

using namespace data;

namespace ui
{
namespace character
{

	spell_list_entry_t::spell_list_entry_t(spell_list_t& _parent, size_t _index)
	: Expandbox(" ", true)
	, parent(_parent)
	, index(_index)
	{
		auto improve_spell = [this](Button&) -> void
		{
			/// TODO: spell improvement
			// if(parent.character.improve_spell(index))
			// {
			// 	learned_spell_t const& learned_spell = parent.character.spells[index];
			// 	effective_skill->setLabel(to_string(learned_spell.get_effective_skill(parent.character)));
			// }
		};
		auto un_improve_spell = [this](Button&) -> void
		{
			/// TODO:
			LOG("TODO: un_improve button callback");
		};

		name            = &mHeader->emplace<Label>("n");
		difficulty      = &mHeader->emplace<Label>("d");
		point_cost      = &mHeader->emplace<Label>("[-]");
		effective_skill = &mHeader->emplace<Label>("-");

		improve_btn     = &mHeader->emplace<Button>("+", std::move(improve_spell) );
		un_improve_btn  = &mHeader->emplace<Button>("-", std::move(un_improve_spell) );

		//properties = &emplace<spell_properties_t>();

		Expandbox& desc_box = emplace<Expandbox>("Description");
		description = &desc_box.emplace<Label>("-");

		set_data(parent.character, index);
		collapse();
	}

	void spell_list_entry_t::set_data(data::character_t const& character, size_t spell_index)
	{
		learned_spell_t const& learned_spell = character.spells[spell_index];
		auto const& spell = learned_spell.spell;
		name->setLabel(spell.name);

		if(spell.difficulty != skill_difficulty_very_hard)
		{
			difficulty->setLabel( "(" + spell.difficulty_string() + ")" );
		}
		else
		{
			difficulty->setLabel(" ");
		}

		point_cost->setLabel( "[" + to_string(learned_spell.point_cost()) + "]  " );
		effective_skill->setLabel(to_string(learned_spell.get_effective_skill(character)));

		// properties->set_data(spell);

		// description->setLabel(spell.description);
		description->setLabel("test description");
	}


	spell_list_t::spell_list_t(data::character_t& _character)
	: Board()
	, character(_character)
	{
		//todo: make the tab rebuild automatically if the number of learned
		//      spells changes
		emplace<Button>("Refresh", [this](Button&){ rebuild(); });

		list = &emplace<List>();

		rebuild();
	}

	void spell_list_t::rebuild()
	{
		list->clear();
		for(size_t i = 0; i < character.spells.size(); ++i)
		{
			list->emplace<spell_list_entry_t>(*this, i);
		}
	}

}
}
}