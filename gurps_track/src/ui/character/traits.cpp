#include "stdafx.h"
#include "traits.h"

#include "data/character.h"

using namespace std;
using namespace mk;

namespace gurps_track
{

using namespace data;

namespace ui
{
namespace character
{

	status_effect_t::status_effect_t(data::status_effect_t& _effect)
	: Label(" ")
	{
		set_data(_effect);
	}

	void status_effect_t::set_data(data::status_effect_t const& effect)
	{
		setLabel(effect.name + " " + effect.get_stat_name() + " " + to_string(effect.amount));
	}

	trait_entry_t::trait_entry_t(std::vector<data::trait_t>& _traits_data, size_t _index)
	: Expandbox(" ", true)
	, traits_data(_traits_data)
	, index(_index)
	{
		name = &mHeader->emplace<Label>("-");
		cost = &mHeader->emplace<Label>("[-]");

		set_data(traits_data[index]);
		collapse();
	}

	trait_entry_t::trait_entry_t(traits_t& _parent, size_t _index)
	: trait_entry_t(_parent.character.traits, _index)
	{
	}

	void trait_entry_t::set_data(data::trait_t& trait)
	{
		name->setLabel(trait.name);
		cost->setLabel("[" + to_string(trait.point_cost) + "]");
		// description->setLabel(trait.description);

		for(size_t i = 0; i < modifiers.size(); ++i)
		{
			vrelease(*(modifiers[i]));
		}
		modifiers.clear();

		for(auto& effect : trait.status_effects)
		{
			modifiers.push_back(&emplace<status_effect_t>(effect));
		}
	}


	traits_t::traits_t(data::character_t& _character)
	: character(_character)
	{
		Sequence& buttons = emplace<Sequence>();
		buttons.emplace<Button>("Add Trait");

		list = &emplace<List>();

		rebuild();
	}

	void traits_t::rebuild()
	{
		list->clear();

		list->emplace<Label>(character.race.name);
		for(size_t i = 0; i < character.race.traits.size(); ++i)
		{
			list->emplace<trait_entry_t>(character.race.traits, i);
		}

		// there's a better way to do this than iterating over the list three times

		list->emplace<Label>("Advantages");
		for(size_t i = 0; i < character.traits.size(); ++i)
		{
			if(character.traits[i].is_advantage())
				list->emplace<trait_entry_t>(*this, i);
		}

		list->emplace<Label>("Disadvantages");
		for(size_t i = 0; i < character.traits.size(); ++i)
		{
			if(character.traits[i].is_disadvantage())
				list->emplace<trait_entry_t>(*this, i);
		}

		list->emplace<Label>("Quirks");
		for(size_t i = 0; i < character.traits.size(); ++i)
		{
			if(character.traits[i].is_quirk())
				list->emplace<trait_entry_t>(*this, i);
		}
	}

}
}
}