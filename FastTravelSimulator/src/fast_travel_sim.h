#pragma once

#include <string>
#include <map>
#include <vector>

#include <boost/rational.hpp>

#include "hex_generator.h"

/// TODO: replace with something slimmer
///       I don't need the 10 or so boost libraries that
///       boost rational includes
using rational_t = boost::rational<int32_t>;


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



    enum path_type_e
    {
        path_paved = 0
      , path_dirt
      , path_none
      , path_forest
      , path_difficult
    };

    /// I could make path_type_e start at -2 and count upwards
    /// but I prefer starting at 0 in case I want to index into
    /// an array (it's common for me to have a constexpr array
    /// of names)
    constexpr int32_t path_difficulty_min = -2;
    inline constexpr int32_t path_difficulty(path_type_e path)
    {
        return path_difficulty_min + path;
    }


    enum travel_pace_e
    {        
          travel_pace_normal
        , travel_pace_quick
        , travel_pace_cautious
        , travel_pace_exploring
        , travel_pace_cautious_exploring
        , travel_pace_foraging
        , travel_pace_count
    };

    /// Experimenting with int percentages instead of floats
    constexpr std::array<int32_t, travel_pace_count> travel_pace_percentages
    {
          100 // travel_pace_normal
        , 150 // travel_pace_quick
        ,  75 // travel_pace_cautious
        ,  50 // travel_pace_exploring
        ,  20 // travel_pace_cautious_exploring
        ,  50 // travel_pace_foraging
    };

    constexpr std::array<char const*, travel_pace_count> travel_pace_strings =
    {
          "normal"
        , "quick"
        , "cautious"
        , "exploring"
        , "cautious exploring"
        , "foraging"
    };

    struct route_segment_t
    {
        hex_coord_t coord;
        path_type_e path = path_none;
        travel_pace_e pace = travel_pace_normal;
        rational_t dist = 0;
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

        int32_t rations_remaining = 0;
        int32_t rations_gained = 0;
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

    struct traveller_t
    {
        std::string name;
        int32_t travel_rate = default_travel_rate;
        int32_t survival_skill = 0;
        int32_t navigation_skill = 0;
        int32_t perception_skill = 0;
    };

    struct travel_group_t
    {
        std::vector<traveller_t> travellers;

        int32_t slowest_travel_rate      = std::numeric_limits<int32_t>::max();
        int32_t highest_survival_skill   = std::numeric_limits<int32_t>::min();
        int32_t highest_navigation_skill = std::numeric_limits<int32_t>::min();

        void add_traveller(traveller_t const& t);
        void add_traveller(traveller_t&& t);
        void adjust_cache(traveller_t const& t);
        void rebuild_cache();
    };

    struct journey_t
    {
        travel_group_t travel_group;
        journey_route_t route;
        int32_t day_rations = 0;

        size_t num_travellers() const
        { return travel_group.travellers.size(); }
    };


    inline std::string to_string(hex_coord_t const& c)
    {
        return "{" + std::to_string(c.x) + "," + std::to_string(c.y) + "}";
    }

    bool is_boring(generated_node_t const&);
    bool is_boring(generated_hex_t const&);
    bool is_boring(journal_entry_t const&);

    int status_effect_value(generated_node_t const&);

    journal_t simulate_journey(journey_t, hex_database_t const&);


    std::string summarize(creature_encounter_t const&);
    std::string summarize(location_encounter_t const&);
    std::string summarize(plant_encounter_t const&);
    std::string summarize(journey_segment_t const&);
    std::string summarize(journal_entry_t const&);
    std::string summarize(journal_t const&);
}