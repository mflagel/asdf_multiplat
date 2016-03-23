#include "stdafx.h"
#include "character_combat.h"

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
	combat_t::combat_t(data::character_t& _character)
	: Board()
	{
		emplace<stats_t>         (_character);
		emplace<encumberance_t>  (_character);
		emplace<armor_t>         (_character);
		emplace<attacks_t>       (_character);
		emplace<defenses_t>      (_character);
		emplace<status_effects_t>(_character);
		emplace<active_spells_t> (_character);
	}


	///
	stats_t::stat_t::stat_t(std::string const& name)
	{
		Expandbox& expnd = emplace<Expandbox>(name, true);
		value_label = &expnd.header()->emplace<Label>("-");
		Sequence& sub_seq = expnd.emplace<Sequence>();
		point_cost_label = &sub_seq.emplace<Label>("[-]");
		sub_seq.emplace<Button>("+"); ///todo: button functionality
		sub_seq.emplace<Button>("-"); ///todo: button functionality

		expnd.collapse();
	}

	stats_t::base_stat_t::base_stat_t(data::character_t& _character, data::base_stat_e _stat)
	: stats_t::stat_t(base_stat_names[_stat])
	, character(_character)
	, stat(_stat)
	{
		set_data(character);
	}

	void stats_t::base_stat_t::set_data(data::character_t const& character)
	{
		value_label->setLabel(to_string(character.base_stats[stat]));
		point_cost_label->setLabel("["
			+ to_string(get_stat_point_cost(character.base_stats[stat]))
			+ "]");
	}

	stats_t::derived_stat_t::derived_stat_t(data::character_t& _character, data::derived_stat_e _stat)
	: stat_t(derived_stat_names[_stat])
	, character(_character)
	, stat(_stat)
	{
		set_data(character);
	}

	void stats_t::derived_stat_t::set_data(data::character_t const& character)
	{
		value_label->setLabel(to_string(character.derived_stats[stat]));
		point_cost_label->setLabel("[todo]");
	}


	///
	stats_t::stats_t(data::character_t& _character)
	: Expandbox{"Stats"}
	{
		GridSheet& stat_grid = emplace<GridSheet>(DIM_X);

		Sequence& st_seq = stat_grid.emplace<Sequence>();
		st 	    = &st_seq.emplace<base_stat_t>(_character, stat_ST);
		fatigue = &st_seq.emplace<derived_stat_t>(_character, stat_fatigue);

		dx = &stat_grid.emplace<base_stat_t>(_character, stat_DX);
		iq = &stat_grid.emplace<base_stat_t>(_character, stat_IQ);

		Sequence& ht_seq = stat_grid.emplace<Sequence>();
		ht 	   = &ht_seq.emplace<base_stat_t>(_character, stat_HT);
		health = &ht_seq.emplace<derived_stat_t>(_character, stat_HP);

		set_data(_character);
	}

	void stats_t::set_data(data::character_t const& character)
	{
		// st->set_data(character);
		// dx->set_data(character);
		// iq->set_data(character);
		// ht->set_data(character);
		fatigue->value_label->setLabel( to_string(character.current_fatigue) + "/" + to_string(character.max_fatigue()) );
		health-> value_label->setLabel( to_string(character.current_HP) + "/" + to_string(character.max_HP()) );
	}


	///
	encumberance_t::encumberance_t(data::character_t& _character)
	: Expandbox("Encumberance")
	{
		GridSheet& grid = emplace<GridSheet>(DIM_X);

		for(size_t i = 0; i < encumberance_count; ++i)
		{
			Sequence& seq = grid.emplace<Sequence>();
			seq.emplace<Label>(data::encumberance_names[i]);
			encumberance_labels[i] = &seq.emplace<Label>("-");
		}
	}

	void encumberance_t::set_data(data::character_t const&)
	{
		//TODO
	}


	///
	armor_t::armor_t(data::character_t& _character)
	: Expandbox("Armor")
	{
		StringVector col_names = {"-", "H", "B", "A", "L", "H", "F"/*, "A"*/};
		Table& table = emplace<Table>( move(col_names), vector<float>(1.0f/data::armor_region_count) );

		// ASSERT(col_names.size() == armor_region_count, "");

		Sequence& pd_seq = table.emplace<Sequence>();
		Sequence& dr_seq = table.emplace<Sequence>();
		pd_seq.emplace<Label>("PD");
		dr_seq.emplace<Label>("DR");

		for(size_t i = 0; i < armor_region_count - 1; ++i)
		{
			pd_labels[i] = &pd_seq.emplace<Label>("-");
			dr_labels[i] = &dr_seq.emplace<Label>("-");
		}

		// other_armor = &emplace<Label>("-");

		set_data(_character);
	}

	void armor_t::set_data(data::character_t const& character)
	{
		for(size_t i = 0; i < character.armor_info.size(); ++i)
		{
			auto const& armor = character.armor_info[i + 1];
			// pd_labels[i]->setLabel(to_string(armor.PD_mod));
			// dr_labels[i]->setLabel(to_string(armor.DR_mod));
		}
	}


	///
	attacks_t::attacks_t(data::character_t&)
	: Expandbox("Attacks")
	{
	}

	void attacks_t::set_data(data::character_t const&)
	{
	}


	///
	defenses_t::defenses_t(data::character_t& _character)
	: Expandbox("Defenses")
	{
		Sequence& dodge_seq = emplace<Sequence>();
		dodge_seq.emplace<Label>("Dodge: ");
		dodge = &dodge_seq.emplace<Label>("-");

		Sequence& block_seq = emplace<Sequence>();
		block_seq.emplace<Label>("Dodge: ");
		block = &block_seq.emplace<Label>("-");

		std::vector<mk::Expandbox*> weapon_parries;


		set_data(_character);
	}

	void defenses_t::set_data(data::character_t const&)
	{
	}


	///
	status_effects_t::status_effects_t(data::character_t& _character)
	: Expandbox("Status Effects")
	, character(_character)
	{
		mHeader->emplace<Button>("Add Effect", [this](Button&){this->add_new_effect();});

		effects = &emplace<List>();

		set_data(character);
	}

	void status_effects_t::set_data(data::character_t const& character)
	{
		effects->clear();

		for(size_t i = 0; i < character.status_effects.size(); ++i)
		{
			effects->emplace<effect_t>(*this, i);
		}
	}

	void status_effects_t::remove_effect(size_t effect_index)
	{
		ASSERT(effect_index < effects->count(), "");

		effects->release(effect_index);
		for(size_t i = effect_index; i < effects->count(); ++i)
		{
			effects->at(effect_index)->as<effect_t>().index -= 1;
		}

		character.remove_effect(effect_index);
	}

	void status_effects_t::add_new_effect()
	{
		// effects->emplace<status_effects_t>(status_effects_t
	}


	///
	status_effects_t::effect_t::effect_t(status_effects_t& _parent, size_t _index)
	: Expandbox("")
	, parent(_parent)
	, index(_index)
	{
		name_textbox = &mHeader->emplace<Textbox>("");
	}

	void status_effects_t::effect_t::set_data(data::status_effect_t const&)
	{

	}


	///
	active_spells_t::active_spells_t(data::character_t&)
	: Expandbox("Active Spells")
	{
	}

	void active_spells_t::set_data(data::character_t const&)
	{
	}

}
}
}