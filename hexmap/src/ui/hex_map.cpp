#include "hex_map.h"

namespace hexmap
{
namespace ui
{
    hex_map_t::hex_map_t()
    {
    }

    constexpr float hex_halfheight = 0.86602540378; //sin(pi/3)
    constexpr std::array<float, 18> hexagon_points =
    {
           1.0f,  0.0f,  0.0f   // mid right
        ,  0.5f,  -hex_halfheight,  0.0f   // bottom right
        , -0.5f,  -hex_halfheight,  0.0f   // bottom left
        , -1.0f,  0.0f,  0.0f   // middle left
        , -0.5f,  hex_halfheight,  0.0f   // top left
        , -0.5f,  hex_halfheight,  0.0f   // top right
    };

    void hex_map_t::render_grid_overlay()
    {
    }
}
}