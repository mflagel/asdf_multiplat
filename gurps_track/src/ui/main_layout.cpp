#include "stdafx.h"
#include "main_layout.h"

#include "ui/skill_list_layout.h"
#include "data/character.h"

using namespace std;
using namespace mk;

namespace gurps_track
{

using namespace data;

namespace ui
{
	struct character_skills_layout_t : mk::Page
	{
		character_skills_layout_t(data::character_t&, data::skill_list_t& all_skills);
	};

	character_skills_layout_t::character_skills_layout_t(data::character_t& _character, skill_list_t& all_skills)
	: Page("Skills")
	{
		Tabber& tabs = emplace<Tabber>();

		Page& learned = tabs.emplace<Page>("Learned");
		learned.emplace<character_skill_list_t>(_character);

		Page& all = tabs.emplace<Page>("All");
		all.emplace<skill_library_t>(all_skills, &_character);
	}

	main_layout_t::main_layout_t(data::character_t& _character, skill_list_t& all_skills)
	: Board()
	{
		Tabber& tabber = emplace<Tabber>();

		Page& character = tabber.emplace<Page>("Character");
		character.emplace<character_main_layout_t>(_character);

		Page& combat    = tabber.emplace<Page>("Stats");
		Page& traits    = tabber.emplace<Page>("Traits");
		Page& skills    = tabber.emplace<character_skills_layout_t>(_character, all_skills);
		Page& spells    = tabber.emplace<Page>("Spells");
	}

}
}