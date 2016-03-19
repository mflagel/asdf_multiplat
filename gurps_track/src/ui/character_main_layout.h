#include <kiui/Ui/mkUiConfig.h>
#include <kiui/Ui/mkUiTypes.h>

namespace gurps_track
{

namespace data
{
	struct character_t;
}

namespace ui
{
	/***  Character Main Layout ***
	+--------------------+
	| Name               |
	| Race               |
	| Date Modified      |
	| *User Defined*     |
	|   ...              |
	+--------------------+
	|                    |
	|    *portrait*      |
	|                    |
	|                    |
	|    *portrait*      |
	|                    |
	+--------------------+
	|-   Point Costs    -|
	|   ...              |
	|   ...              |
	+--------------------+
	|-   EXP Info Log   -|
	| +4   Nov 20        |
	| -2   Lev 17->20    |
	|-0.5  Alchemy       |
	|  ...               |
	|Unspent Pts: ##  [+][-]|  //does not collapse
	+---------------------+
	*/


	struct title_display_t : mk::Expandbox
	{
		title_display_t(data::character_t*);
	};

	struct character_portrait_t : mk::Expandbox
	{
		character_portrait_t(data::character_t*);
	};

	struct point_cost_summary_t : mk::Expandbox
	{
		enum cost_categories_e
		{
		      stats = 0
		    // , race
		    , advantages
		    , disadvantages
		    , quirks
		    , skills
		    , spells
		    , total
		    , category_count
		};

		static constexpr std::array<const char*, point_cost_summary_t::category_count> category_names =
		{
		      "Stats"
		    // , "Race"
		    , "Advantages"
		    , "Disadvantages"
		    , "Quirks"
		    , "Skills"
		    , "Spells"
		    , "Total"
		};

		std::array<mk::Label*, category_count> cost_labels;

		point_cost_summary_t(data::character_t*);
		void set_data(data::character_t*);
	};

	struct experience_log_t : mk::Expandbox
	{
		experience_log_t(data::character_t*);
	};

	struct character_main_layout_t : mk::Sheet
	{
		character_main_layout_t(data::character_t*);
	};
}
}