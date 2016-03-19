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
    |     Spells      |
    |- Air              -|
    | Name   2  13       |
    |-Name   1  12       |
    |  11 + 1(bonus desc)|
    | Name   8  15 (P/E) |
    |-Name   0  13 (P/E) |
    |   Default: X       |
    | Name   4  12 (P/H) |
    |  
    |- Movement         -|
    |  Name 1 14 (M/VH) |
    |  Name 1 15        |
    +-----------------+
    */
    struct character_spells_layout_t : ui_list_view_t
    {
        character_spells_layout_t(character_t* character);
    };
}
}

#pragma clang diagnostic pop
