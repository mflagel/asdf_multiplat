#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"

#include "stdafx.h"
#include "character.h"
#include "ui_wrapper.h"

namespace asdf
{
namespace gurps_track
{

    struct character_t;


    /*
    +-----------------+
    |-  Advantages   -|
    |-*Attractive 1   |
    |-*Charisma 1     |
    |-*Combat Reflexes|
    |- Extra Fatigue 4|
    |- Literacy       |
    |-*Magery 3       |
    |-*Musical Abil 2 |
    |-*Unaging        |
    +-----------------+
    |- Disadvantages -|
    |-*Code of Honor  |
    |- Fanatacism     |
    |- Megalomaniac   |
    |- Mild Curiosity |
    |-*Sense of Duty  |
    +-----------------+
    |-    Quirks     -|
    | Dislikes Auth   |
    | Showoff         |
    | Obnoxious Drunk |
    | Mild Aracnophobe|
    | Entertainer     |
    +-----------------+
    */


    struct trait_listing_t : collapsable_list_view_t
    {
        trait_listing_t(trait_t const& trait);
    };

    struct character_traits_layout_t : ui_list_view_t
    {
        collapsable_list_view_t* advantages = nullptr;
        collapsable_list_view_t* disadvantages = nullptr;
        collapsable_list_view_t* quirks = nullptr;

        character_traits_layout_t(character_t* character);
    };
}
}

#pragma clang diagnostic pop
