#include "stdafx.h"
#include "character_layout.h"

#include "character_main_layout.h"
#include "character_combat_layout.h"
#include "character_traits_layout.h"
#include "character_skills_layout.h"
#include "character_spells_layout.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace gurps_track
{
    // using namespace basic_info;

    // namespace
    // {
        const color_t background_color = COLOR_DARKGREY;
        constexpr char info_font[] = "arial";

        constexpr size_t major_padding = 8.0f;
        constexpr size_t minor_padding = 2.0f;
    // }

    ///
    character_layout_t::character_layout_t(shared_ptr<character_t> _character)
    : ui_view_t(vec2(), vec2(400.0f, 600.0f))
    , character{std::move(_character)}
    {
        /// background
        add_child<ui_image_t>(background_color, vec2(), vec2(), ui_align_fill);

        // auto menu_bar = add_child<ui_list_view_t>();
        // menu_bar->vertical = false;
        // menu_bar->alignment = ui_align_top_stretch;
        // menu_bar->set_size(1.0, 42.0f);

        // menu_bar->add_cell<button_view_t>("New",     [this](ui_view_t*){new_file_callback();});
        // menu_bar->add_cell<button_view_t>("Open",    [this](ui_view_t*){open_file_callback();});
        // menu_bar->add_cell<button_view_t>("Save",    [this](ui_view_t*){save_file_callback();});
        // menu_bar->add_cell<button_view_t>("Save As", [this](ui_view_t*){save_as_file_callback();});

        tabs_container = add_child<ui_list_view_t>();
        tabs_container->vertical = false;
        tabs_container->alignment = ui_align_top_stretch;
        tabs_container->set_size(1.0f, 42.0f);

        tab_buttons[tab_main]   = tabs_container->add_cell<button_view_t>("M",  [this](ui_base_t*){set_tab(tab_main);});
        tab_buttons[tab_combat] = tabs_container->add_cell<button_view_t>("C",  [this](ui_base_t*){set_tab(tab_combat);});
        tab_buttons[tab_traits] = tabs_container->add_cell<button_view_t>("T",  [this](ui_base_t*){set_tab(tab_traits);});
        tab_buttons[tab_skills] = tabs_container->add_cell<button_view_t>("Sk", [this](ui_base_t*){set_tab(tab_skills);});
        tab_buttons[tab_spells] = tabs_container->add_cell<button_view_t>("Sp", [this](ui_base_t*){set_tab(tab_spells);});

        for(auto& btn : tab_buttons)
        {
            btn->btn->togglable = true;
            btn->set_size(62.0f, 1.0f);
        }


        container = add_child<ui_view_t>();
        container->alignment = ui_align_top;
        container->offset.y -= major_padding + tabs_container->size.y;
        container->set_size(size - vec2(20.0f));

        main   = container->add_child<character_main_layout_t>(  character);
        combat = container->add_child<character_combat_layout_t>(character.get());
        traits = container->add_child<character_traits_layout_t>(character.get());
        skills = container->add_child<character_skills_layout_t>(character);
        spells = container->add_child<character_spells_layout_t>(character.get());

        tab_pages[0] = main;
        tab_pages[1] = combat;
        tab_pages[2] = traits;
        tab_pages[3] = skills;
        tab_pages[4] = spells;

        for(auto& page : tab_pages)
        {
            page->alignment = ui_align_top_stretch;
            page->set_size(size - vec2(20));
        }

        set_tab(tab_main);
    }

    void character_layout_t::align()
    {
        // container->set_size(size - vec2(20.0f));
        set_size(vec2(size.x, container->size.y + major_padding + major_padding));

        ui_view_t::align();
    }

    void character_layout_t::set_tab(layout_tab_e tab)
    {
        for(auto& btn : tab_buttons)
        {
            btn->btn->toggled = false;
        }

        for(auto& page : tab_pages)
        {
            page->visible = false;
        }

        tab_buttons[tab]->btn->toggled = true;
        tab_pages[tab]->visible = true;
    }

    void character_layout_t::new_file_callback()
    {

    }
    void character_layout_t::open_file_callback()
    {

    }
    void character_layout_t::save_file_callback()
    {
        character->save_data();
    }
    void character_layout_t::save_as_file_callback()
    {

    }

}
}