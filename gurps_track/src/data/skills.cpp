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
	constexpr std::array<int, 6> get_skill_cost_curve(skill_difficulty_e difficulty)
	{
	    if(is_physical(difficulty))
	    {
	        return physical_skill_cost_curve;
	    }
	    else if(is_mental(difficulty))
	    {
	        if(difficulty == mental_very_hard)
	        {
	            return mental_VH_skill_cost_curve;
	        }

	        return mental_skill_cost_curve;
	    }

	    return physical_skill_cost_curve; //shouldn't happen
	}

	size_t get_skill_cost_index(skill_difficulty_e difficulty, int point_cost)
	{
	    int index = 0;
	    auto curve_array = get_skill_cost_curve(difficulty);

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

	int point_cost_from_index(skill_difficulty_e difficulty, size_t cost_index)
	{
		auto curve = get_skill_cost_curve(difficulty);

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

    int get_skill_improvement_cost(skill_difficulty_e difficulty, size_t cost_index_start, size_t cost_index_end)
	{
		LOG_IF(cost_index_start == cost_index_end, "getting skill improvement cost for an improvement of zero");
		
		//lazy
		return point_cost_from_index(difficulty, cost_index_end) - point_cost_from_index(difficulty, cost_index_start);
	}

	bool skill_t::is_physical() const
	{
		return ::gurps_track::data::is_physical(difficulty);
	}
	bool skill_t::is_mental() const
	{
		return ::gurps_track::data::is_mental(difficulty);
	}

	learned_skill_t::learned_skill_t(skill_t _skill, int _initial_improvement_level)
	: skill(std::move(_skill))
	, num_improvements(_initial_improvement_level)
	{
	}

	int learned_skill_t::point_cost() const
	{
		return point_cost_from_index(skill.difficulty, num_improvements);
	}

	int learned_skill_t::improvement_cost() const
	{
		return point_cost_from_index(skill.difficulty, num_improvements + 1) - point_cost();
	}

    int learned_skill_t::get_effective_skill(int stat_value, int modifiers) const
    {
    	return stat_value + skill_start_value[skill.difficulty] + num_improvements + modifiers;
    }

    /// Todo: handle skills that default off of ST or HT
    int learned_skill_t::get_effective_skill(character_t* character) const
    {
    	int stat_value = 0;

    	if(skill.is_physical())
    	{
    		stat_value = character->DX();
    	}
    	else if(skill.is_mental())
    	{
    		stat_value = character->IQ();
    	}

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