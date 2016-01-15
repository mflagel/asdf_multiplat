#include "stdafx.h"
#include "character_skills_layout.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace gurps_track
{
	using namespace ui::layout_constants;

    character_skills_layout_t::character_skills_layout_t(std::shared_ptr<character_t> _character)
    : ui_list_view_t()
    , character(_character)
    {
    	// add_cell<label_t>("Skills");
    	skill_library = add_cell<character_skill_library_t>(character);

    	skill_library->set_size(100, 400);
    }


    character_skill_library_entry_t::character_skill_library_entry_t(character_skill_library_t* _library, size_t _index)
    : collapsable_list_view_t("test_name")
    , skill_name(button->label)
    , library(_library)
    , index(_index)
    {
        point_cost      = button->add_child<ui_label_t>("0.5", Content.fonts[info_font]);
        effective_skill = button->add_child<ui_label_t>("00", Content.fonts[info_font]);

        extra_info = add_cell<label_t>("(W/W) 00 + 0");

        increment_button = extra_info->add_child<button_view_t>("+", [this](ui_base_t*){ this->increment_point_value(); } );
        decrement_button = extra_info->add_child<button_view_t>("-", [this](ui_base_t*){ this->decrement_point_value(); } );
        
        skill_name->	 alignment = ui_align_left;
        point_cost->     alignment = ui_align_right;
        effective_skill->alignment = ui_align_right;

        extra_info->label->alignment = ui_align_left;
        extra_info->label->offset.x = minor_padding;

        skill_name->	 offset.x = minor_padding;
        point_cost->	 offset.x = -96;
        effective_skill->offset.x = -50;

        increment_button->alignment = ui_align_right;
        increment_button->set_size(40,40);
        increment_button->offset.x = -60.0f - minor_padding;

        decrement_button->alignment = ui_align_right;
        decrement_button->set_size(40,40);

        set_data(library->character->skills[index]);
    }

    void character_skill_library_entry_t::set_data(skill_listing_t const& skill_listing)
    {
        skill_name->     set_text(skill_listing.skill.name);

        if(skill_listing.point_cost() == 0)
        	point_cost->set_text("(0.5)");
        else
        	point_cost->set_text("(" + std::to_string(skill_listing.point_cost()) + ")" );

        int skill_value = skill_listing.get_effective_skill	(library->character.get());
        effective_skill->set_text(std::to_string(skill_value));

        std::string extra_info_str = "(" + std::string(skill_difficulty_abbreviations[skill_listing.skill.difficulty]) + ") ";
        //todo: handle display of bonuses
        extra_info->set_text(std::move(extra_info_str));
    }

    void character_skill_library_entry_t::change_point_value(bool positive)
    {
    	library->character->improve_skill(index);
        set_data(library->character->skills[index]);
    }

    void character_skill_library_entry_t::increment_point_value()
    {
    	change_point_value(true);
    }

    void character_skill_library_entry_t::decrement_point_value()
    {
    	LOG("todo: character_skill_library_entry_t::decrement_point_value()");
    	// change_point_value(false);
    }


    character_skill_library_t::character_skill_library_t(std::shared_ptr<character_t> _character)
    : ui_list_view_t()
    , character(std::move(_character))
    {
    	physical_skills = add_cell<collapsable_list_view_t>("Physical");
    	  mental_skills = add_cell<collapsable_list_view_t>("Mental");

        rebuild_list();
    }

    void character_skill_library_t::rebuild_list()
    {
        //todo: do something more optimal than destroying all cells and rebuilding
        physical_skills->cells.erase(physical_skills->cells.begin() + 1, physical_skills->cells.end());
		  mental_skills->cells.erase(  mental_skills->cells.begin() + 1,   mental_skills->cells.end());

        for(size_t i = 0; i < character->skills.size(); ++i)
        {
        	if(character->skills[i].skill.is_physical())
        	{
            	physical_skills->add_cell<character_skill_library_entry_t>(this, i);
            }
            else
            {
            	mental_skills->add_cell<character_skill_library_entry_t>(this, i);
            }
        }

        top_parent()->align();
    }

    void character_skill_library_t::update(float dt)
    {
        if(prev_skill_count != character->skills.size())
        {
            rebuild_list();
        }

        prev_skill_count = character->skills.size();
    }
}
}