#include "hex_util.h"

#include <array>
#include <glm/glm.hpp>
#include <asdf_multiplat/main/asdf_defs.h>

namespace asdf
{
namespace hexmap
{
    //big collection of util functions from
    //http://www.redblobgames.com/grids/hexagons/


    /// Coord Translations
    axial_coord_t cube_to_axial(cube_coord_t cube_coords)
    {
        return axial_coord_t{ cube_coords.x
                            , cube_coords.z };
    }

    cube_coord_t axial_to_cube(axial_coord_t axial_coords)
    {
        auto x = axial_coords.x;
        auto z = axial_coords.y;
        auto y = -x - z;

        return cube_coord_t(x,y,z);
    }

    hex_coord_t cube_to_oddq(cube_coord_t cube_coords)
    {
        return hex_coord_t{ cube_coords.x
                          , cube_coords.z + (cube_coords.x - (cube_coords.x & 1)) / 2 // bitwise and checks for odd column
                          };
    }

    cube_coord_t oddq_to_cube(hex_coord_t hex_coord)
    {
        auto x = hex_coord.x;
        auto z = hex_coord.y - (hex_coord.x - (hex_coord.x & 1)) / 2;
        auto y = -x - z;

        return cube_coord_t{x, y, z};
    }

    // //alias func
    // inline cube_coord_t hex_to_cube(hex_coord_t hex_coord)
    // {
    //     return oddq_to_cube(hex_coord);
    // }
    // inline hex_coord_t cube_to_hex(cube_coord_t cube_coord)
    // {
    //     return cube_to_oddq(cube_coord);
    // }


    /// Distance
    float cube_distance(cube_coord_t a, cube_coord_t b)
    {
        return (abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z)) / 2;
    }

    float axial_distance(axial_coord_t a, axial_coord_t b)
    {
        auto ac = hex_to_cube(a);
        auto bc = hex_to_cube(b);
        return cube_distance(ac, bc);
    }

    float hex_distance(hex_coord_t a, hex_coord_t b)
    {
        auto ac = hex_to_cube(a);
        auto bc = hex_to_cube(b);
        return cube_distance(ac, bc);
    }

    /// Direction
    // cube_coord_t cube_neighbor(cube_coord_t coord, hexagon_direction_e direction)
    // {
    //     return coord + hexagon_cube_directions[direction];
    // }


    /// Ranges
    std::vector<cube_coord_t> cube_range(cube_coord_t center, int range)
    {
        std::vector<cube_coord_t> results;

        for(int dx = -range; dx <= range; ++dx)
        {
            for(int dy = glm::max(-range, -dx - range); dy <= glm::min(range, -dx + range); ++dy)
            {
                auto dz = -dx - dy;
                results.push_back(center + cube_coord_t(dx, dy, dz));
            }
        }

        return results;
    }


    /// Other
    std::vector<cube_coord_t> cube_ring(cube_coord_t center, int radius)
    {
        std::vector<cube_coord_t> results;
        results.reserve(radius * 6);

        auto cube = center + (hexagon_cube_directions[hex_dir_down_left] * radius);

        for(int edge_num = 0; edge_num < 6; ++edge_num) //for each hex edge
        {
            for(int edge_hex_num = 0; edge_hex_num < radius; ++edge_hex_num)
            {
                results.push_back(cube);
                cube = cube_neighbor(cube, static_cast<hexagon_direction_e>(edge_num));
            }
        }

        return results;
    }

    std::vector<cube_coord_t> cube_spiral(cube_coord_t center, int radius)
    {
        std::vector<cube_coord_t> results;

        // Fills a hex region with rings
        for(size_t cur_radius = 1; cur_radius < radius; ++cur_radius)
        {
            auto ring = cube_ring(center, cur_radius);

            results.reserve(results.size() + ring.size());
            std::move(ring.begin(), ring.end(), std::back_inserter(results));
        }

        return results;
    }

}
}