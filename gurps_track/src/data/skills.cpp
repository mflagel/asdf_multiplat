#include "stdafx.h"
#include "skills.h"

#include <asdfm/utilities/utilities.h>
#include <cJSON/cJSON.h>

#include "character.h"

using namespace asdf;

namespace gurps_track
{
namespace data
{
	constexpr std::array<int, 6> get_skill_cost_curve(skill_difficulty_e difficulty, base_stat_e stat)
	{
	    if(stat == stat_IQ)
	    {
	    	if(difficulty == skill_difficulty_very_hard)
	    	{
	    	    return mental_VH_skill_cost_curve;
	    	}

	    	return mental_skill_cost_curve;
	    }
	    else
	    {
	        return physical_skill_cost_curve;
	    }
	}

	size_t get_skill_cost_index(skill_difficulty_e difficulty, base_stat_e stat, int point_cost)
	{
	    int index = 0;
	    auto curve_array = get_skill_cost_curve(difficulty, stat);

	    while(index < curve_array.size())
	    {
	        index += point_cost < curve_array[index];
	    }

	    int cur_cost = curve_array.back();
	    while(point_cost < cur_cost)
	    {
	        cur_cost += curve_array.back();
	        ++index;
	    }

	    return index;
	}

	int point_cost_from_index(skill_difficulty_e difficulty, base_stat_e stat, size_t cost_index)
	{
		auto curve = get_skill_cost_curve(difficulty, stat);

		if(cost_index < curve.size())
		{
			return curve[cost_index];
		}
		else
		{
		   // everything past curve.size() is incremented on by the diff
		   // between the last and second last elements
		   // example: if grabbing cost for physical_hard level 10:
		   //          16 + (16-8) * (1 + 10 - physical_curve.size())

			ASSERT(curve.size() > 1, "");
			auto diff = curve[curve.size() - 1] - curve[curve.size() - 2];

			return curve[curve.size() - 1] + (diff * (1 + cost_index - curve.size()));
		}
	}

    int get_skill_improvement_cost(skill_difficulty_e difficulty, base_stat_e stat, size_t cost_index_start, size_t cost_index_end)
	{
		LOG_IF(cost_index_start == cost_index_end, "getting skill improvement cost for an improvement of zero");
		
		//lazy
		return point_cost_from_index(difficulty, stat, cost_index_end) - point_cost_from_index(difficulty, stat, cost_index_start);
	}

	bool skill_t::is_physical() const
	{
		return base_stat != stat_IQ;
	}
	bool skill_t::is_mental() const
	{
		return base_stat == stat_IQ;
	}
	std::string skill_t::difficulty_string() const
	{
		return std::string(base_stat_names[base_stat]) + "/" + std::string(skill_difficulty_abbreviations[difficulty]);
	}

	std::string spell_t::difficulty_string() const
	{
		return std::string(base_stat_names[stat_IQ]) + "/" + std::string(skill_difficulty_abbreviations[difficulty]);
	}

    /// Todo: handle skills that default off of ST or HT
    int learned_skill_t::get_effective_skill(character_t const& character) const
    {
    	int stat_value = character.get_stat(skill.base_stat);

    	//todo: handle skill bonuses from traits or special rules
    	int modifiers = 0;

    	return get_effective_skill(stat_value, modifiers);
    }

    int learned_spell_t::get_effective_skill(character_t const& character) const
    {
    	int stat_value = character.IQ();// + character.magery();

    	//todo: handle skill bonuses from traits or special rules
    	int modifiers = 0;

    	return get_effective_skill(stat_value, modifiers);
    }


    skill_list_t::skill_list_t(std::string const& filepath)
    {
		LOG("loading skills from %s", filepath.c_str());
	    std::string json_str = util::read_text_file(filepath);
	    cJSON* root = cJSON_Parse(json_str.c_str());

	    ASSERT(root, "");
	    from_JSON(root);
	    cJSON_Delete(root);

	    LOG("Skills Loaded: %zu", this->size());
    }
}
}