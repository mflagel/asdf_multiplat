#include "stdafx.h"
#include "skill_list_layout.h"

#include "cJSON/cJSON.h"

#include "character.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace gurps_track
{

// namespace ui
// {
    using namespace ui::layout_constants;

    skill_library_entry_t::skill_library_entry_t(skill_library_t* _library, size_t _index)
    : collapsable_list_view_t("")
    , library(_library)
    , index(_index)
    {
        add_button = button->add_child<button_view_t>("+", [this](ui_base_t*){this->add_callback();});
        difficulty = button->add_child<ui_label_t>("", Content.fonts[info_font]);
        
        alignment = ui_align_horizontal_stretch;
        add_button->alignment = ui_align_left;
        button->label->alignment = ui_align_left;
        difficulty->alignment = ui_align_right;

        // description = add_cell<label_t>("");

        set_data(library->skills[index]);
    }

    void skill_library_entry_t::align()
    {
        difficulty->offset.x = -40;

        float thing = button->size.y;
        // float thing = 40.0f;

        add_button->set_size(thing,thing);

        // button->btn->expansion_delta.x = -thing * 10;
        // button->btn->offset.x = thing / 2.0f;

        button->label->offset.x = add_button->size.x + minor_padding;

        ui_list_view_t::align();

        button->btn->alignment = ui_align_right;
        button->btn->set_size(glm::vec3(button->size.x - thing, button->size.y, 0.0f));
        button->btn->align();
    }

    void skill_library_entry_t::add_callback()
    {
        library->add_skill_to_character(index);
        add_button->enabled = false;
    }


    /*

     */
    void skill_library_entry_t::set_data(skill_t const& skill)
    {
        button->set_text(skill.name);// + " (" + skill_difficulty_abbreviations[skill.difficulty] + ")");
        // difficulty->set_text("asdf");
        difficulty->set_text( "(" + std::string(skill_difficulty_abbreviations[skill.difficulty]) + ")");
        // description->set_text(skill.description);
    }


    skill_library_t::skill_library_t(std::shared_ptr<character_t> _character)
    : ui_list_view_t()
    , character(std::move(_character))
    {
        load_from_file( ASSET_PATH("data/gurps_skills.json") );

        for(size_t i = 0; i < skills.size(); ++i)
        {
            add_cell<skill_library_entry_t>(this, i);
        }
    }

    void skill_library_t::load_from_file(std::string const& filepath)
    {
        std::string json_str = util::read_text_file(filepath);
        cJSON* root = cJSON_Parse(json_str.c_str());

        ASSERT(root, "");
        from_JSON(root);
        cJSON_Delete(root);
    }

    void skill_library_t::add_skill_to_character(size_t skill_index)
    {
        character->learn_skill(skills[skill_index], 1);
    }

// }
}
}