#include <kiui/Ui/mkUiConfig.h>
#include <kiui/Ui/mkUiTypes.h>

#include "character/overview.h"
#include "character/combat.h"
#include "character/skill_list.h"

namespace gurps_track
{

namespace data
{
	struct character_t;
}

namespace ui
{

	struct main_layout_t : mk::Board
	{
		main_layout_t(data::character_t&, data::skill_list_t& skills);
	};

}
}