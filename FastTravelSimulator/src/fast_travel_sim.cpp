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
        constexpr char const* creature_encounter_data_filepath = "../data/CreatureEncounter.json";
        constexpr char const* dist_units_name = "miles";
    }

    // in order to figure out how far someone can go on a route
    // that crosses differing terrain difficulties, convert a distance
    // and slow-rate into a larger distance 
    int32_t difficulty_modified_dist(int32_t dist, int32_t difficulty)
    {
        WARN_IF(difficulty < 0, "negative difficulty will give negative (or zero) modified distance, which is probably bad");
        return dist * (1 + difficulty);
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

        const int32_t daily_travel_dist = num_hours_travelling * default_travel_rate;
        int32_t daily_dist_remaining = num_hours_travelling * default_travel_rate;

        for(size_t current_seg_index = 0; current_seg_index < route.size(); ++current_seg_index)
        {
            auto const& cur_seg = route[current_seg_index];
            auto const& cur_hex = hex_db.hex_at(cur_seg.coord);
            journey_segment_t jseg(cur_seg);

            auto modified_dist = difficulty_modified_dist(cur_seg.dist, cur_hex);

            // if there is travel budget left, travel this segment
            if(daily_dist_remaining >= modified_dist)
            {
                jseg.locations.push_back(cur_hex.location);
                jseg.plants.push_back(cur_hex.plant);

                current_journal_entry.segments.push_back(std::move(jseg));

                daily_dist_remaining -= modified_dist;
            }
            else
            {
                /// FIXME safe conversion for int <--> float
                // split segment into travellable and not-travellable
                int32_t untravelled_dist = modified_dist - daily_dist_remaining;
                float fract_travelled = float(modified_dist - untravelled_dist) / float(modified_dist);
                int32_t actual_travelled = int32_t(float(cur_seg.dist) * fract_travelled);
                int32_t actual_untravelled = cur_seg.dist - actual_travelled;

                ASSERT(untravelled_dist > 0, "");
                ASSERT(fract_travelled > 0.0f && fract_travelled < 1.0f, "");
                ASSERT(actual_travelled > 0, "");
                ASSERT(actual_untravelled > 0, "");

                //add journal entry of current segment with distance travelled
                current_journal_entry.segments.push_back(std::move(jseg));
                current_journal_entry.segments.back().dist = actual_travelled;

                //zero the dist remaining so we actually 'travel' it
                //otherwise infinite loop
                daily_dist_remaining = 0;

                //push new seg with remaining distance
                route_segment_t new_seg{cur_seg.coord, cur_seg.path, actual_untravelled};
                route.insert(route.begin() + current_seg_index + 1, std::move(new_seg));
            }


            // 'make camp' if there's no more travel remaining for the day
            //          or if this is the last segment
            if(daily_dist_remaining == 0 || current_seg_index + 1 == route.size())
            {
                //todo: encounters for the day
                auto creature_gen_node = generate_node(hex_db.creature_rollables);
                current_journal_entry.creatures.push_back(std::move(creature_gen_node));

                journal.push_back(std::move(current_journal_entry));


                ++days_spent_travelling;
                daily_dist_remaining = num_hours_travelling * default_travel_rate;
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

        summary += "--- Journey Stats ---\n";
        summary += "The Journey took " + std::to_string(journal.size()) + " days total\n";

        std::vector<plant_encounter_t> all_plants;

        uint32_t day_index = 0;
        for(auto const& entry : journal)
        {
            for(auto const& jseg : entry.segments)
                all_plants.insert(all_plants.end(), jseg.plants.begin(), jseg.plants.end());

            summary += "-- Day " + std::to_string(day_index + 1) + " --  ";
                summary += summarize(entry);
            ++ day_index;
        }

        summary += "\n\n-- Top 3 Noteworthy Plants --\n";

        sort_by_worth(all_plants);
        for(size_t i = 0; i < 3; ++i)
            summary += summarize(all_plants[i]) + "\n";


        return summary;
    }
}

using namespace fast_travel_sim;

const std::array<hex_t, 8> test_hexes =
{
    hex_t{hex_coord_t{0,0}, terrain_flat},
    hex_t{hex_coord_t{1,0}, terrain_flat},
    hex_t{hex_coord_t{2,0}, terrain_hilly},
    hex_t{hex_coord_t{3,0}, terrain_hilly},
    hex_t{hex_coord_t{4,0}, terrain_hilly},
    hex_t{hex_coord_t{5,0}, terrain_mountainous},
    hex_t{hex_coord_t{6,0}, terrain_mountainous},
    hex_t{hex_coord_t{7,0}, terrain_mountainous}
};

const std::array<route_segment_t, 8> test_route =
{
    route_segment_t{hex_coord_t{0,0}, path_paved,     miles_per_hex},
    route_segment_t{hex_coord_t{1,0}, path_paved,     miles_per_hex},
    route_segment_t{hex_coord_t{2,0}, path_dirt,      miles_per_hex},
    route_segment_t{hex_coord_t{3,0}, path_dirt,      miles_per_hex},
    route_segment_t{hex_coord_t{4,0}, path_none,      miles_per_hex},
    route_segment_t{hex_coord_t{5,0}, path_forest,    miles_per_hex},
    route_segment_t{hex_coord_t{6,0}, path_forest,    miles_per_hex},
    route_segment_t{hex_coord_t{7,0}, path_difficult, miles_per_hex}
};


int main(int argc, char* argv[])
{
    hex_database_t hex_db;

    //TODO: take this as an argument?
    hex_db.hex_rollables = node_from_json(hex_encounter_data_filepath);
    hex_db.creature_rollables = node_from_json(creature_encounter_data_filepath);

    //TODO: load route from file
    journey_route_t route;
    route.insert(route.begin(), test_route.begin(), test_route.end());

    for(auto const& hex : test_hexes)
    {
        hex_db.generate_if_empty(hex);
    }

    journal_t results = make_long_journey(route, hex_db);
    cout << summarize(results);

    return 0;
}