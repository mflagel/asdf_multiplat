#include <kiui/Ui/mkUiConfig.h>
#include <kiui/Ui/mkUiTypes.h>

#include "data/basic_data.h"
#include "data/character.h"

namespace gurps_track
{

namespace ui
{
namespace character
{
	/*
	+-----------------+
	| ST  8  FT  8/8  |
	| DX 11           |
	| IQ 14           |
	| HT 10  HP 10/10 |
	+-----------------+
	| Spd: 6.25  Mv 6 |
	| Init: X  Enc: H |
	+-----------------+
	|- Encumberance  -|
	| None:           |
	| Light:          |
	| Medium:         |
	| ....            |
	+-----------------+
	|-    Armour     -|
	|   H B A L H F A |
	|PD 0 2 2 1 2 2   |
	|DR 0 3 2 1 2 2   |
	|other pd other dr|
	+-----------------+
	|-    Damage     -|
	|Thr: 1d-1  Sw:1d |
	|Kick: 1d-1       |
	+-----------------+
	|-    Attacks    -|
	|- Large Knife   -|
	|   Cut           |
	|    {cut stats}  |
	|   Imp           |
	|    {imp stats}  |
	|- Small Knife   -|
	|- Dagger        -|
	|                 |
	|- Poltergeist   -|
	|   {ranged stats}|
	|                 |
	|- Ammunition    -|
	+-----------------+
	|-   Defenses    -|
	| Dodge: 6 + PD   |
	|-Parry: 7 + PD  -|  // default collapsed
	|  Weapon 1 Parry |
	|  Weapon 2 Parry |
	|  Weapon 3 Parry |
	| Block: 5 + PD   |
	+-----------------+
	|- Status Effects-|
	| Poisoned        |
	|   HP -= 2/s     |
	| Deafened        |
	+-----------------+
	|- Active Spells -|
	|  Penalty: -##   |
	|-  Illusions ## -|
	|- Simple  2      |
	|    Description  |
	|      +5  +3A    |  // spell pass by 5, art pass by 3
	|    Description  |
	|      +1  -4A    |
	|- Complex  1     |
	|    Description  |
	|      +2  +2A    |  // spell pass by 5, art pass by 3
	|- Keen Eyes 3    |
	|    Description  |  // ex: when spell was casted
	|      +2         |
	|- Levitation     |
	|    Weight: 90lb |
	|    +10          |
	+-----------------+
	*/

	struct stats_t : mk::Expandbox
	{
		struct stat_t : mk::Sequence
		{
			mk::Label* value_label;
			mk::Label* point_cost_label;

			stat_t(std::string const& name);
		};

		struct base_stat_t : stat_t
		{
			data::character_t& character;
			data::base_stat_e stat;

			base_stat_t(data::character_t&, data::base_stat_e);
			void set_data(data::character_t const&);
		};

		struct derived_stat_t : stat_t
		{
			data::character_t& character;
			data::derived_stat_e stat;

			derived_stat_t(data::character_t&, data::derived_stat_e);
			void set_data(data::character_t const&);
		};

		stat_t* st = nullptr;
		stat_t* dx = nullptr;
		stat_t* iq = nullptr;
		stat_t* ht = nullptr;

		stat_t* fatigue = nullptr;
		stat_t* health  = nullptr;

		stats_t(data::character_t&);
		void set_data(data::character_t const&);
	};

	struct movement_t : mk::Expandbox
	{
		mk::Label* speed        = nullptr;
		mk::Label* move         = nullptr;
		mk::Label* initiative   = nullptr;
		mk::Label* encumberance = nullptr;

		movement_t(data::character_t&);
	};

	struct encumberance_t : mk::Expandbox
	{
		std::array<mk::Label*, data::encumberance_count> encumberance_labels;

		encumberance_t(data::character_t&);
		void set_data(data::character_t const&);
	};

	struct armor_t : mk::Expandbox
	{
		std::array<mk::Label*, data::armor_region_count-1> pd_labels;  // -1 to ignore armor_region_none
		std::array<mk::Label*, data::armor_region_count-1> dr_labels;
		mk::Label* other_armor;

		armor_t(data::character_t&);
		void set_data(data::character_t const&);
	};

	struct attacks_t : mk::Expandbox
	{
		std::vector<mk::Expandbox*> attack_boxes;

		attacks_t(data::character_t&);
		void set_data(data::character_t const&);
	};

	struct defenses_t : mk::Expandbox
	{
		mk::Label* dodge = nullptr;
		mk::Label* block = nullptr;
		std::vector<mk::Expandbox*> weapon_parries;

		defenses_t(data::character_t&);
		void set_data(data::character_t const&);
	};

	struct status_effects_t : mk::Expandbox
	{
		struct effect_t : mk::Expandbox
		{
			mk::Textbox* name_textbox = nullptr;
			std::vector<mk::Label*> modifiers;

			status_effects_t& parent;
			size_t index;

			effect_t(status_effects_t&, size_t _index);
			void set_data(data::status_effect_t const&);
			void remove_effect();
		};

		mk::Sheet* effects = nullptr;
		
		data::character_t& character;

		status_effects_t(data::character_t&);
		void set_data(data::character_t const&);

		void add_new_effect();
		void remove_effect(size_t effect_index);
	};

	struct active_spells_t : mk::Expandbox
	{
		struct active_spell_t : mk::Expandbox
		{
			mk::Label* description = nullptr;
			std::vector<mk::Label> modifiers;
		};

		mk::Label* num_active_spells = nullptr;

		active_spells_t(data::character_t&);
		void set_data(data::character_t const&);
	};


	struct combat_t : mk::ScrollSheet
	{
		combat_t(data::character_t&);
	};
}
}
}