#include "stdafx.h"
#include "main_layout.h"

#include "data/character.h"

using namespace std;
using namespace mk;

namespace gurps_track
{

using namespace data;

namespace ui
{

	main_layout_t::main_layout_t(data::character_t& _character)
	: Board()
	{
		Tabber& tabber = emplace<Tabber>();

		Page& character = tabber.emplace<Page>("Character");
		character.emplace<character_main_layout_t>(_character);

		Page& combat    = tabber.emplace<Page>("Stats");

		Page& traits    = tabber.emplace<Page>("Traits");

		Page& skills    = tabber.emplace<Page>("Skills");
		skills.emplace<Button>("Add Skills");
		character.emplace<character_skill_list_t>(_character);

		Page& spells    = tabber.emplace<Page>("Spells");
	}

}
}