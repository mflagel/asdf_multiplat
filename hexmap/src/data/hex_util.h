#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <asdf_multiplat/main/asdf_defs.h>

namespace asdf
{
namespace hexmap
{

    //big collection of util functions from
    //http://www.redblobgames.com/grids/hexagons/

    using cube_coord_t  = glm::ivec3;
    using axial_coord_t = glm::ivec2;
    using hex_coord_t   = glm::ivec2;
    using grid_coord_t  = hex_coord_t;


    /// Coord Translations
    axial_coord_t cube_to_axial(cube_coord_t cube_coords);
    cube_coord_t axial_to_cube(axial_coord_t axial_coords);
    hex_coord_t cube_to_oddq(cube_coord_t cube_coords);
    cube_coord_t oddq_to_cube(hex_coord_t hex_coord);

    //alias func
    inline cube_coord_t hex_to_cube(hex_coord_t hex_coord)
    {
        return oddq_to_cube(hex_coord);
    }
    inline hex_coord_t cube_to_hex(cube_coord_t cube_coord)
    {
        return cube_to_oddq(cube_coord);
    }


    /// Distance
    float cube_distance(cube_coord_t a, cube_coord_t b);
    float axial_distance(axial_coord_t a, axial_coord_t b);
    float hex_distance(hex_coord_t a, hex_coord_t b);

    /// Direction
    enum hexagon_direction_e
    {
        hex_dir_down_right = 0
      , hex_dir_up_right
      , hex_dir_up
      , hex_dir_up_left
      , hex_dir_down_left
      , hex_dir_down
    };

    /*constexpr*/const std::array<cube_coord_t, 6> hexagon_cube_directions =
    {
        cube_coord_t{+1, -1,  0} //down right
      , cube_coord_t{+1,  0, -1} //up right
      , cube_coord_t{ 0, +1, -1} //up
      , cube_coord_t{-1, +1,  0} //up left
      , cube_coord_t{-1,  0, +1} //down left
      , cube_coord_t{ 0, -1, +1} //down
    };

    inline cube_coord_t cube_neighbor(cube_coord_t coord, hexagon_direction_e direction)
    {
        return coord + hexagon_cube_directions[direction];
    }


    /// Ranges
    std::vector<cube_coord_t> cube_range(cube_coord_t center, int range);


    /// Other
    std::vector<cube_coord_t> cube_ring(cube_coord_t center, int radius);
    std::vector<cube_coord_t> cube_spiral(cube_coord_t center, int radius);

}
}