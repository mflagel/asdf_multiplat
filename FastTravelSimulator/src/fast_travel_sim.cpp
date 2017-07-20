#include "fast_travel_sim.h"

#include <plantgen/plant_generator.h>
#include <plantgen/from_json.h>

using namespace std;
using namespace plantgen;


namespace fast_travel_sim
{
    namespace
    {
        constexpr char const* hex_encounter_data_filepath = "../data/HexData.json";
    }

    // in order to figure out how far someone can go on a route
    // that crosses differing terrain difficulties, convert a distance
    // and slow-rate into a larger distance 
    int32_t difficulty_modified_dist(int32_t dist, int32_t difficulty)
    {
        return dist * difficulty;
    }

    int32_t difficulty_modified_dist(int32_t dist, hex_t const& hex)
    {
        return difficulty_modified_dist(dist, terrain_difficulty(hex));
    }

    int32_t difficulty_modified_dist(route_segment_t const& route_seg, hex_database_t const& hex_db)
    {
        return difficulty_modified_dist(route_seg.dist, hex_db.hex_at(route_seg.coord));
    }


    void hex_database_t::generate_if_empty(hex_t const& hex)
    {
        hex_data.try_emplace(hex.coord, hex, hex_rollables);
    }

    void hex_database_t::generate_if_empty(std::vector<hex_t> const& hexes)
    {
        for(auto const& hex : hexes)
        {
            generate_if_empty(hex);
        }
    }


    generated_hex_t::generated_hex_t(hex_t const& _hex, pregen_node_t const& pregen_node)
    : hex_t(_hex)
    {
        auto gen_node = generate_node(pregen_node);

        /// TODO: search for children by name instead of assuming location
        ASSERT(gen_node.children.size() == 2, "expected hex generation node to have properties");
        location = gen_node.children[0];
        plant = gen_node.children[1];
    }




    // bool is_boring(generated_hex_t const& h)
    // {
    //     return h.location == ""
    //         && h.plant == ""
    //         && h.creature == "";
    // }

    // ///TODO: implement once I have a better understanding of
    // ///      what a journal_entry_t is going to be 
    // bool is_boring(journal_entry_t const& e)
    // {
    //     //is_boring(e.hex);
    //     return false;
    // }



    /// FIXME I think I'll have to create some sort of fake coords
    /// because otherwise it makes no sense to take a single hex
    /// and distance, since route_segment_t stores a coord
    journey_route_t build_route(int distance_miles, hex_t hex)
    {
        auto d = std::ldiv(distance_miles, miles_per_hex);
        uint32_t num_hexes = d.quot + d.rem > 0;

        journey_route_t route;
        route.resize(num_hexes, route_segment_t{hex.coord, path_none, miles_per_hex});

        return route;
    }


    // uint32_t hours_required(route_segment_t const& segment)
    // {
    //     //segment.dist
    // }
    

    /*
    for each segment
      travel as far as possible in the time remaining for the day
        split the segment if only part of it is travelled
      if the travel budget is empty
        add a journal entry for the day
    */

    //take route by copy, as it might get modified later if I split a segment
    journal_t make_long_journey(journey_route_t route, hex_database_t const& hex_db)
    {
        journal_t journal;
        journal_entry_t current_journal_entry;
        uint32_t days_spent_travelling = 0;

        int32_t daily_dist_remaining = num_hours_travelling * default_travel_rate;

        for(size_t current_seg_index = 0; current_seg_index < route.size(); ++current_seg_index)
        {
            auto const& cur_seg = route[current_seg_index];
            auto const& cur_hex = hex_db.hex_at(cur_seg.coord);

            auto modified_dist = difficulty_modified_dist(cur_seg.dist, cur_hex);

            // if there is travel budget left, travel this segment
            if(daily_dist_remaining >= modified_dist)
            {
                current_journal_entry.hexes.push_back(cur_seg);
                daily_dist_remaining -= modified_dist;
            }
            else
            {
                // split segment into travellable and not-travellable
                int32_t untravelled_dist = modified_dist - daily_dist_remaining;
                int32_t fraction = daily_dist_remaining / modified_dist;

                route_segment_t new_seg{cur_seg.coord, cur_seg.path, untravelled_dist};
                route.insert(route.begin() + current_seg_index + 1, std::move(new_seg));
            }


            // make camp if there's no more travel remaining for the day
            if(daily_dist_remaining == 0)
            {
                //todo: encounters for the day


                journal.emplace_back(std::move(current_journal_entry));  /// current_journal_entry will be empty after being moved
                ++days_spent_travelling;
            }
        }


        return journal;
    }




    std::string summarize(route_segment_t const& seg)
    {
        std::string summary;

        summary += to_string(seg.coord) + ":";

        // if(is_boring(hex))
        // {
        //     summary += " nothing interesting";
        // }

        return summary;
    }

    std::string summarize(journal_entry_t const& entry)
    {
        std::string summary;

        auto num_hexes = convert_integer<size_t, uint64_t>(entry.hexes.size());
        summary += std::to_string(num_hexes) + " hexes traveled\n";

        for(auto const& hex : entry.hexes)
        {
            summary += summarize(hex) + "\n";
        }

        return summary;
    }

    std::string summarize(journal_t const& journal)
    {
        std::string summary;

        summary += "Journey Stats:\n";
        summary += "    The Journey took " + std::to_string(journal.size()) + " days";

        uint32_t day_index = 0;

        for(auto const& entry : journal)
        {
            summary += "Day " + std::to_string(day_index + 1) + ":";
                summary += summarize(entry);
        }

        return summary;
    }
}

using namespace fast_travel_sim;

const std::array<hex_t, 4> test_hexes =
{
    hex_t{hex_coord_t{0,0}, terrain_flat},
    hex_t{hex_coord_t{1,0}, terrain_flat},
    hex_t{hex_coord_t{2,0}, terrain_flat},
    hex_t{hex_coord_t{3,0}, terrain_flat}
};

const std::array<route_segment_t, 4> test_route =
{
    route_segment_t{hex_coord_t{0,0}, path_none, miles_per_hex},
    route_segment_t{hex_coord_t{1,0}, path_none, miles_per_hex},
    route_segment_t{hex_coord_t{2,0}, path_none, miles_per_hex},
    route_segment_t{hex_coord_t{3,0}, path_none, miles_per_hex}
};


int main(int argc, char* argv[])
{
    hex_database_t hex_db;

    //TODO: take this as an argument?
    hex_db.hex_rollables = node_from_json(hex_encounter_data_filepath);

    //TODO: load route from file
    journey_route_t route = build_route(miles_per_hex * 10, hex_t()); //test route

    for(auto const& hex : test_hexes)
    {
        hex_db.generate_if_empty(hex);
    }

    journal_t results = make_long_journey(route, hex_db);
    cout << summarize(results);

    return 0;
}