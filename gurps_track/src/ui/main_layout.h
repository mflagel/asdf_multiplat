#include <kiui/Ui/mkUiConfig.h>
#include <kiui/Ui/mkUiTypes.h>

#include "character_main_layout.h"
#include "character_skill_list.h"

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
		main_layout_t(data::character_t&);
	};

}
}