#include "stdafx.h"
#include "skills.h"

namespace asdf
{
namespace gurps_track
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
			return curve[curve.size() - 1] * (1 + cost_index - curve.size());
		}
	}

    int get_skill_improvement_cost(skill_difficulty_e difficulty, int point_cost_start, int point_cost_end)
	{
		LOG_IF(point_cost_start == point_cost_end, "getting skill improvement cost for an improvement of zero");

		int improvement_cost = 0;
		// size_t cur_index = get_skill_cost_index(cur_point_cost, difficulty);

		ASSERT(false, "todo: get_skill_improvement_cost()");
		// for(size_t i = cur_index; i < cur_index + num_improvements; ++i)
		// {
		// 	improvement_cost += 
		// }
	}

	bool skill_t::is_physical() const
	{
		return ::asdf::gurps_track::is_physical(difficulty);
	}
	bool skill_t::is_mental() const
	{
		return ::asdf::gurps_track::is_mental(difficulty);
	}

	skill_listing_t::skill_listing_t(skill_t _skill, int _initial_improvement_level)
	: skill(std::move(_skill))
	, num_improvements(_initial_improvement_level)
	{
	}

	int skill_listing_t::point_cost() const
	{
		return point_cost_from_index(skill.difficulty, num_improvements);
	}

	int skill_listing_t::improvement_cost() const
	{
		return point_cost_from_index(skill.difficulty, num_improvements + 1);
	}

    int skill_listing_t::get_effective_skill(int stat_value, int modifiers) const
    {
    	return stat_value + skill_start_value[skill.difficulty] + num_improvements + modifiers;
    }

    /// Todo: handle skills that default off of ST or HT
    int skill_listing_t::get_effective_skill(character_t* character) const
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
}
}