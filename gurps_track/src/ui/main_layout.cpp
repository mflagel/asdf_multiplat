#include "stdafx.h"
#include "main_layout.h"

#include "data/character.h"
#include "skill_list.h"
#include "character/overview.h"
#include "character/combat.h"
#include "character/traits.h"
#include "character/skill_list.h"

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

	skills_layout_t::skills_layout_t(data::character_t& _character, data::skill_list_t& all_skills)
	: Page("Skills")
	{
		Tabber& tabs = emplace<Tabber>();

		Page& learned = tabs.emplace<Page>("Learned");
		learned.emplace<ui::character::skill_list_t>(_character);

		Page& all = tabs.emplace<Page>("All");
		all.emplace<ui::skill_list_t>(all_skills, &_character);
	}

	main_layout_t::main_layout_t(data::character_t& _character, data::skill_list_t& all_skills)
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
		Page& spells    = tabber.emplace<Page>("Spells");
	}

}
}