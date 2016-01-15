#pragma once

#include "skills.h"
#include "ui_wrapper.h"

struct cJSON;

namespace asdf
{
namespace gurps_track
{

    struct character_t;

// namespace ui
// {
    struct skill_library_t;

    /*
    +---------------------+
    |-Skill Name(P/E) |+|-|
    |  Description        |
    +---------------------+
    */
    struct skill_library_entry_t : collapsable_list_view_t
    {
        ui_label_t* difficulty;
        button_view_t* add_button;

        label_t* description;


        skill_library_t* library;
        size_t index;

        skill_library_entry_t(skill_library_t* library, size_t index);

        void set_data(skill_t const&);

        virtual void align() override;

        void add_callback();
    };


    /*
    +---------------------+
    |-|+|Skill Name (P/E)-|
    |  Description        |
    |-|+|SkillA     (P/A)-|
    |-|+|SkillZZ    (P/A)-|
    +---------------------+
    */
    struct skill_library_t : ui_list_view_t
    {
        std::shared_ptr<character_t> character;
        std::vector<skill_t> skills;

        skill_library_t(std::shared_ptr<character_t> _character);

        void load_from_file(std::string const& filepath);

        void add_skill_to_character(size_t skill_index);

        void from_JSON(cJSON*);
    };

// }
}
}