#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <asdf_multiplat/main/asdf_defs.h>

namespace asdf
{
namespace hexmap
{
    /// Hexagon Base Dimensions
    constexpr float hex_width    = 1.0f;
    constexpr float hex_width_d2 = hex_width / 2.0f;
    constexpr float hex_width_d4 = hex_width_d2 / 2.0f;

    constexpr float hex_height    = 0.86602540378f; //sin(pi/3)
    constexpr float hex_height_d2 = hex_height / 2.0f;
    constexpr float hex_height_d4 = hex_height_d2 / 2.0f;

    constexpr float hex_edge_length = hex_width * 0.52359877559f;  //width * sin(pi/6)

    constexpr float px_per_unit = 128.0f;
    constexpr float units_per_px = 1.0f / px_per_unit;

    constexpr std::array<float, 18> hexagon_points =
    {
           hex_width_d2,   0.0f,           0.0f   // mid right
        ,  hex_width_d4,  -hex_height_d2,  0.0f   // bottom right
        , -hex_width_d4,  -hex_height_d2,  0.0f   // bottom left
        , -hex_width_d2,   0.0f,           0.0f   // middle left
        , -hex_width_d4,   hex_height_d2,  0.0f   // top left
        ,  hex_width_d4,   hex_height_d2,  0.0f   // top right
    };

    /// Coord Types
    using cube_coord_t  = glm::ivec3;
    using axial_coord_t = glm::ivec2;
    using hex_coord_t   = glm::ivec2;


    enum hex_region_e
    {
          hex_no_region
        , hex_top_left
        , hex_top_right
        , hex_left
        , hex_right
        , hex_bottom_left
        , hex_bottom_right
        , hex_center
        , num_hex_regions
    };


    /// Coord Conversions
    hex_coord_t world_to_hex_coord(glm::vec2 world_pos);
    glm::vec2 hex_to_world_coord(hex_coord_t hex_coord, bool odd_q = false);


    /// Other
    enum hex_snap_points_e
    {
          hex_snap_none         = 0
        , hex_snap_center       = 1
        , hex_snap_vertex       = 2
        // , hex_snap_edge_nearest = 4
        // , hex_snap_edge_center  = 8

        , hex_snap_center_and_verts = hex_snap_center | hex_snap_vertex
        , hex_snap_all         = 0xFFFFFFFF
    };
    using hex_snap_flags_t = uint32_t;

    glm::vec2 nearest_snap_point(glm::vec2 const& pos_world, hex_snap_flags_t);




    /// big collection of util functions from
    /// http://www.redblobgames.com/grids/hexagons/

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