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

    // constexpr int32_t default_travel_rate = 24; //24 miles per day
    constexpr int32_t default_travel_rate = 3; //24 miles per day
    constexpr int32_t miles_per_hex = 6; // TEMP will make this variable later

    struct route_segment_t
    {
        hex_coord_t coord;
        path_type_e path = path_none;
        int32_t dist = 0;
    };

    using journey_route_t = std::vector<route_segment_t>;

    struct journey_segment_t : route_segment_t
    {
        std::vector<location_encounter_t> locations;
        std::vector<plant_encounter_t> plants;

        journey_segment_t(route_segment_t const& rseg)
        : route_segment_t(rseg)
        {}
    };

    // represents a day's worth of travel
    struct journal_entry_t
    {
        std::vector<journey_segment_t> segments;
        std::vector<creature_encounter_t> creatures;
    };

    using journal_t = std::vector<journal_entry_t>;
    

    /// WIP
    struct daily_travel_schedule_t
    {
        uint32_t num_hours_travelling = 8;
        uint32_t num_hours_sleeping   = 8;
        uint32_t num_hours_surviving  = 8;

        inline bool is_valid() const {
            return num_hours_travelling
                 + num_hours_sleeping
                 + num_hours_surviving
                == 24;
        }
    };


    inline std::string to_string(hex_coord_t const& c)
    {
        return "{" + std::to_string(c.x) + "," + std::to_string(c.y) + "}";
    }

    bool is_boring(generated_node_t const&);
    bool is_boring(generated_hex_t const&);
    bool is_boring(journal_entry_t const&);

    int status_effect_value(generated_node_t const&);



    journey_route_t build_route(int distance_miles, hex_t);
    journal_t make_long_journey(journey_route_t, hex_database_t const&);


    std::string summarize(creature_encounter_t const&);
    std::string summarize(location_encounter_t const&);
    std::string summarize(plant_encounter_t const&);
    std::string summarize(journey_segment_t const&);
    std::string summarize(journal_entry_t const&);
    std::string summarize(journal_t const&);
}