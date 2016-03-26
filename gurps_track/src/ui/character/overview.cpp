#include "overview.h"

#include <Ui/mkUiConfig.h>
#include <Ui/mkUiTypes.h>

#include <Ui/mkUi.h>

#include "data/character.h"

using namespace std;
using namespace mk;

namespace gurps_track
{

using namespace data;

namespace ui
{
namespace character
{
	// +-----------------+
	// | Name            |
	// | Race            |
	// | Date Modified   |
	// +-----------------+
	title_display_t::title_display_t(character_t& _character)
	: Expandbox("Character Title")
	{
		emplace<Label>(string("Name: " + _character.name));
		emplace<Label>(string("Description: " + _character.description));
		emplace<Label>(string("Race: " + _character.race.name));
		// emplace<Label>(string("#: " + to_string(character.version) + " | " + date.to_string());  //FIXME
	}


	portrait_t::portrait_t(character_t& _character)
	: Expandbox("Portrait")
	{
		//todo: emplace an image
	}


	// +-----------------+
	// | Point Costs     |
	// | Stats:      70  |
	// | Advantages: 30  |
	// |  ...            |
	// +-----------------+
	constexpr std::array<const char*, point_cost_summary_t::category_count> point_cost_summary_t::category_names;

	point_cost_summary_t::point_cost_summary_t(character_t& _character)
	: Expandbox("Point Cost Summary")
	{
		StringVector table_titles(point_cost_summary_t::category_names.begin(), point_cost_summary_t::category_names.end());

		Table& table = emplace<Table>(StringVector({"Type", "Amount"}), std::vector<float>({ 0.5f, 0.5f}));

		for(size_t i = 0; i < category_count; ++i)
		{
			LabelSequence& sequence = table.emplace<LabelSequence>(StringVector({category_names[i]}));
			cost_labels[i] = &( sequence.emplace<Label>("--") );
		}

		set_data(_character);
	}

	void point_cost_summary_t::set_data(character_t& character)
	{
		int adv_total = 0;
		int disadv_total = 0;
		int quirk_total = 0;
		for(auto const& trait : character.traits)
		{
		    if(trait.is_advantage())
		    {
		        adv_total += trait.point_cost;
		    }
		    if(trait.is_disadvantage())
		    {
		        disadv_total += trait.point_cost;
		    }
		    if(trait.is_quirk())
		    {
		        quirk_total += trait.point_cost;
		    }
		}

		cost_labels[stats]->        setLabel(to_string(character.stats_point_cost()));
		// cost_labels[race]->         setLabel(to_string(character.race.point_cost()));
		cost_labels[advantages]->   setLabel(to_string(adv_total));
		cost_labels[disadvantages]->setLabel(to_string(disadv_total));
		cost_labels[quirks]->       setLabel(to_string(quirk_total));
		cost_labels[skills]->       setLabel(to_string(static_cast<int>(character.skills_point_cost())));
		cost_labels[spells]->       setLabel(to_string(character.spells_point_cost()));
		//lazy, should save totals I grabbed before to the stack and sum them here rather than re-loop
		cost_labels[total]->        setLabel(to_string(character.total_point_cost()));
	}


	experience_log_t::experience_log_t(character_t& _character)
	: Expandbox("Experience Log")
	{
		for(auto const& transaction : _character.point_transactions)
		{
			emplace<Label>(to_string(transaction.amount) + " (" + transaction.description + ")");
		}

		emplace<Label>("Unspent Points: " + to_string(_character.unspent_points));
	}


	overview_t::overview_t(character_t& _character)
	: Sheet()
	{
		this->emplace<title_display_t>     (_character);
		this->emplace<portrait_t>          (_character);
		this->emplace<point_cost_summary_t>(_character);
		this->emplace<experience_log_t>    (_character);
	}

}
}
}