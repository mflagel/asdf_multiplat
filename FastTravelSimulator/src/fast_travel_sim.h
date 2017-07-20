#pragma once

#include <string>
#include <map>
#include <vector>

#include "hex_generator.h"



// http://thealexandrian.net/wordpress/17320/roleplaying-games/hexcrawl-part-2-wilderness-travel
// http://thealexandrian.net/wordpress/17333/roleplaying-games/hexcrawl-part-4-encounter-tables


namespace fast_travel_sim
{
    //assumed schedule
    constexpr uint32_t num_hours_travelling = 8;
    constexpr uint32_t num_hours_sleeping   = 8;
    constexpr uint32_t num_hours_surviving  = 8;
                                    //total = 24

    constexpr int32_t default_travel_rate = 24; //24 miles per day
    constexpr int32_t miles_per_hex = 6; // TEMP will make this variable later

    struct route_segment_t
    {
        hex_coord_t coord;
        path_type_e path = path_none;
        int32_t dist = 0;
    };

    using journey_route_t = std::vector<route_segment_t>;


    struct journal_entry_t
    {
        std::vector<route_segment_t> hexes;
    };

    using journal_t = std::vector<journal_entry_t>;
    

    //int travel_speed(hex_t const&


    inline std::string to_string(hex_coord_t const& c)
    {
        return "{" + std::to_string(c.x) + "," + std::to_string(c.y) + "}";
    }
    bool is_boring(generated_hex_t const&);
    bool is_boring(journal_entry_t const&);



    journey_route_t build_route(int distance_miles, hex_t);
    journal_t make_long_journey(journey_route_t, hex_database_t const&);



    std::string summarize(route_segment_t const&);
    std::string summarize(journal_entry_t const&);
    std::string summarize(journal_t const&);
}