#include "hex_util.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <asdf_multiplat/main/asdf_defs.h>

#include "data/hex_grid.h"

using namespace glm;

namespace asdf
{
namespace hexmap
{
    /*        __
    \  0,1  /   
     \ ___ /  1,1
     /     \    
  hex  0,0  \ __
world  0,0  /
     \ ___ /  1,0
           \
            \ __
          
    world (0,0) is the center of the (0,0)th hexagon. add hexagon halfsize to the world pos to make world 0,0 the bottom left of the hexagon
    hex coords range from (-0.5,-0.5) to (0.5,0.5)

    everything within the angled slashes could be in the left or right hex depending
    on the y coord. the edges are at 60 degree angles (60 and 180-60)

    relevant edge can be determined by y position
            
    */
    glm::ivec2 world_to_hex_coord(glm::vec2 world_pos)
    {
        //adjust so that world 0,0 is the bottom left of hexagon 0,0
        world_pos += vec2(hex_width_d2, hex_height_d2);

        //convert mouse world coords to a hexagon coord
        float sub_column = world_pos.x / hex_width_d4;
        float sub_row    = world_pos.y / hex_height_d2;

        //LOG("subcol: %f   subrow: %f", sub_column, sub_row);

        int column = static_cast<int>(glm::floor(sub_column / 3.0f));
        int row    = static_cast<int>(glm::floor(world_pos.y / hex_height));

        //if column is within hex_width_d4 of the column center (ie: fraction is +- 0.25) then it's only one column
        if(static_cast<int>(glm::floor(sub_column)) % 3 == 0) //horizontal overlap every 3 sub-columns (with a width of one sub-column)
        {
            //todo: handle column overlap

            bool even = static_cast<int>(std::abs(floor(sub_row))) % 2 == 0;
            //even rows slant right  '/'
            //odd rows slant left    '\'

            vec2 line;

            auto angle = (even * 1.0f + PI) / 3.0f;
            line.x = cos(angle);
            line.y = sin(angle);
            line *= hex_edge_length;

            vec2 p0(floor(sub_column) * hex_width_d4, floor(sub_row) * hex_height_d2); //bottom point of slant

            auto p1 = p0 + line;
            auto const& p2 = world_pos;
            auto side = (p1.x - p0.x)*(p2.y - p0.y) - (p1.y - p0.y)*(p2.x - p0.x);  //FIXME
            //((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x))
            
            
            //LOG("side: %f", side);
            column -= 1 * (side < 0);
        }

        //if odd column, adjust row down
        if(column % 2 == 1)
        {
            row = static_cast<int>(glm::floor((world_pos.y + hex_height_d2) / hex_height));
        }

        return ivec2(column, row);
    }

    glm::vec2 hex_to_world_coord(glm::ivec2 hex_coord, bool odd_q)
    {
        vec2 world_coord{hex_coord};

        world_coord.x -= hex_width_d4 * hex_coord.x; //horizontal overlap
        world_coord.y *= hex_height; //scale vertically since hexes aren't one unit tall

        world_coord.y -= hex_height_d2 * !odd_q * (hex_coord.x & 1); //vertical offset for even hexes
        world_coord.y -= hex_height_d2 *  odd_q * (hex_coord.x+1 & 1); //vertical offset for odd hexes

        return world_coord;
    }    


    glm::vec2 nearest_snap_point(glm::vec2 const& world_pos, hex_snap_flags_t snap_flags)
    {
        using namespace glm;

        if(snap_flags == hex_snap_none)
            return world_pos;

        
        //get the center point of the current hex
        ivec2 hx = world_to_hex_coord(world_pos);
         vec2 hex_world = hex_to_world_coord(hx);

        /// get all possible snap points and return the closest one
        /// could optimize by ignoring snap points beyond threshhold
        std::vector<vec2> snap_points;

        /// center
        if((snap_flags & hex_snap_center) > 0)
        {
            snap_points.push_back(hex_world);
        }

        /// vertex
        if((snap_flags & hex_snap_vertex) > 0)
        {
            for(size_t vert_ind = 0; vert_ind < 6; ++vert_ind)
            {
                //local space
                vec2 vert_pos(hexagon_points[vert_ind*3 + 0]
                            , hexagon_points[vert_ind*3 + 1]
                            );

                vert_pos += hex_world; //push into world space

                snap_points.push_back(vert_pos);
            }
        }

        /// edge nearest

        /// edge center


        if(snap_points.empty())
            return world_pos;


        std::sort(snap_points.begin(), snap_points.end(), 
            [world_pos](vec2 const& lhs, vec2 const& rhs) -> bool
            {
                auto ldist = abs(distance(lhs, world_pos));
                auto rdist = abs(distance(rhs, world_pos));

                return ldist < rdist;
            });

        return snap_points[0];
    }    



    /// big collection of util functions from
    /// http://www.redblobgames.com/grids/hexagons/


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