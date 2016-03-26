#include <kiui/Ui/mkUiConfig.h>
#include <kiui/Ui/mkUiTypes.h>

#include "data/skills.h"

namespace gurps_track
{

namespace data
{
	struct character_t;
}

namespace ui
{
namespace character
{
	struct skill_list_t;

	/*
	+---------------------+
	|-Skill Name (0.5) 12-|
	|  (P/E) 11 + 1 [+][-]|
	|  +1 Combat Reflexes |
	|  Description        |
	+---------------------+
	 */
	struct skill_list_entry_t : mk::Expandbox
	{
		mk::Label*  name            = nullptr;
		mk::Label*  difficulty      = nullptr;
		mk::Label*  point_cost      = nullptr;
		mk::Label*  effective_skill = nullptr;
		mk::Button* improve_btn     = nullptr;
		mk::Button* un_improve_btn  = nullptr;

		mk::Label* modifier_info = nullptr; //ex: '+1 from Combat Reflexes' or 'defaulted from Throwing'
		mk::Label& description;

		skill_list_t& parent;
		size_t index = 900001;

		skill_list_entry_t(skill_list_t& _parent, size_t index);

		void set_data(data::character_t const&, size_t skill_index);
	};

	/*
    +-----------------+
    |- Phsysical        -|
    | Name   2  13 (P/E) |
    |-Name  0.5 12       |
    |  11 + 1(bonus desc)|
    | Name   8  15 (P/E) |
    |-Name   0  13 (P/E) |
    |   Default: X       |
    | Name   4  12 (P/H) |
    |                    |
    |- Mental           -|
    |  First Aid 0 R 15 (M/A) |
    |  ... 0.5 14 (M/A) |
    +-----------------+
    */
	struct skill_list_t : mk::Board
	{
		mk::List* scroll_list;

		data::character_t& character;

		skill_list_t(data::character_t& _character);

		void rebuild();
	};

}
}
}