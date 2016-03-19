#include "stdafx.h"
#include "character_traits_layout.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace gurps_track
{
    trait_listing_t::trait_listing_t(trait_t const& trait)
    : collapsable_list_view_t(trait.name, false)
    {
        auto pt_cost = button->add_child<label_t>(to_string(trait.point_cost));
        pt_cost->alignment = ui_align_right;

        add_cell<label_t>(trait.description);

        for(auto const& effect : trait.status_effects)
        {
            add_cell<label_t>(effect.name);

            ///FIXME
            // add_cell<labeled_value_t>(base_stat_names[effect.relevant_stat], effect.amount);
        }
    }

    character_traits_layout_t::character_traits_layout_t(character_t* character)
    : ui_list_view_t()
    {
        advantages    = add_cell<collapsable_list_view_t>("Advantages");
        disadvantages = add_cell<collapsable_list_view_t>("Disadvantages");
        quirks        = add_cell<collapsable_list_view_t>("Quirks");

        auto add_trait = [this](trait_t const& trait)
        {
            if(trait.is_advantage())
            {
                advantages->add_cell<trait_listing_t>(trait);
            }
            if(trait.is_disadvantage())
            {
                disadvantages->add_cell<trait_listing_t>(trait);
            }
            if(trait.is_quirk())
            {
                quirks->add_cell<trait_listing_t>(trait);
            }
        };

        for(auto const& trait : character->race.traits)
        {
            add_trait(trait);
        }
        for(auto const& trait : character->traits)
        {
            add_trait(trait);
        }
    }
}
}