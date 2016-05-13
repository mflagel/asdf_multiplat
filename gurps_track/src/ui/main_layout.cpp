#include "stdafx.h"
#include "main_layout.h"

#include "data/character.h"
#include "ui/skill_list.h"
#include "ui/spell_list.h"
#include "character/overview.h"
#include "character/combat.h"
#include "character/traits.h"
#include "character/skill_list.h"
#include "character/spell_list.h"

using namespace std;
using namespace mk;

namespace gurps_track
{

using namespace data;

namespace ui
{
	struct skills_layout_t : mk::Page
	{
		skills_layout_t(data::character_t&, data::skill_list_t& all_skills);
	};

	struct spells_layout_t : mk::Page
	{
		spells_layout_t(data::character_t&, data::spell_list_t& all_spells);
	};

	skills_layout_t::skills_layout_t(data::character_t& _character, data::skill_list_t& all_skills)
	: Page("Skills")
	{
		Tabber& tabs = emplace<Tabber>();

		Page& learned = tabs.emplace<Page>("Learned");
		learned.emplace<ui::character::skill_list_t>(_character);

		Page& all = tabs.emplace<Page>("All");
		all.emplace<ui::skill_list_t>(all_skills, &_character);
	}

	spells_layout_t::spells_layout_t(data::character_t& _character, data::spell_list_t& all_spells)
	: Page("Spells")
	{
		Tabber& tabs = emplace<Tabber>();

		Page& learned = tabs.emplace<Page>("Learned");
		learned.emplace<ui::character::spell_list_t>(_character);

		Page& all = tabs.emplace<Page>("All");
		all.emplace<ui::spell_list_t>(all_spells, &_character);
	}

	main_layout_t::main_layout_t(data::character_t& _character, data::skill_list_t& all_skills, data::spell_list_t& all_spells)
	: Board()
	{
		Tabber& tabber = emplace<Tabber>();

		Page& character = tabber.emplace<Page>("Overview");
		character.emplace<character::overview_t>(_character);

		Page& combat    = tabber.emplace<Page>("Combat");
		combat.emplace<character::combat_t>(_character);

		Page& traits    = tabber.emplace<Page>("Traits");
		traits.emplace<character::traits_t>(_character);

		Page& skills    = tabber.emplace<skills_layout_t>(_character, all_skills);

		Page& spells    = tabber.emplace<spells_layout_t>(_character, all_spells);
	}

}
}