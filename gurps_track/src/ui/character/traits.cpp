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

	trait_entry_t::trait_entry_t(traits_t& _parent, size_t _index)
	: Expandbox(" ", true)
	, parent(_parent)
	, index(_index)
	{
		name = &mHeader->emplace<Label>("-");
		cost = &mHeader->emplace<Label>("[-]");

		set_data(parent.character.traits[index]);
		collapse();
	}

	void trait_entry_t::set_data(data::trait_t const& trait)
	{
		name->setLabel(trait.name);
		cost->setLabel("[" + to_string(trait.point_cost) + "]");
		// description->setLabel(trait.description);
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
		for(size_t i = 0; i < character.traits.size(); ++i)
		{
			list->emplace<trait_entry_t>(*this, i);
		}
	}

}
}
}