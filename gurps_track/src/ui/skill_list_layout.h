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

	struct skill_library_t;

	/*
	+---------------------+
	|-Skill Name(P/E) |+|-|
	|  Description        |
	+---------------------+
	*/
	struct skill_library_entry_t : mk::Expandbox
	{
		mk::Label* name        = nullptr;
		mk::Label* difficulty  = nullptr;
		mk::InputBool* add_remove_button = nullptr;

		mk::Label& description;

		skill_library_t& parent;
		size_t index;

		skill_library_entry_t(skill_library_t& _parent, size_t index);

		void set_data(data::skill_t const&);
	};

	/*
	+---------------------+
	|-|+|Skill Name (P/E)-|
	|  Description        |
	|-|+|SkillA     (P/A)-|
	|-|+|SkillZZ    (P/A)-|
	+---------------------+
	*/
	struct skill_library_t : mk::Sheet
	{
		data::skill_list_t const& skills;
		data::character_t* character = nullptr;

		skill_library_t(data::skill_list_t const&, data::character_t* _character = nullptr);
	};
}
}