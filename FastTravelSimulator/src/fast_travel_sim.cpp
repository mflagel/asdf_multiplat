#include "fast_travel_sim.h"

using namespace std;

namespace fast_travel_sim
{
    namespace
    {
        constexpr std::string hex_encounter_data_filepath = "../data/HexEncounter.json"
    }


    void map_data_t::generate_if_empty(hex_t const& hex)
    {
        hex_database.try_emplace(hex.coord, hex, hex_generation_data);
    }

    void map_data_t::generate_if_empty(std::vector<hex_t> const& hexes)
    {
        for(auto const& hex : hexes)
        {
            generate_if_empty(hex.coord);
        }
    }


    generated_hex_t::generated_hex_t(hex_t const& _hex, pregen_node_t const& pregen_node)
    : hex_t(_hex)
    {
        //TODO:
    }




    bool is_boring(generated_hex_t const& h)
    {
        return h.location == ""
            && h.plant == ""
            && h.creature == "";
    }

    bool is_boring(journal_entry_t const& e)
    {
        is_boring(e.hex);
    }




    journey_route_t build_route(int distance_miles, hex_t hex)
    {
        auto d = std::ldiv(distance_miles, miles_per_hex);

        uint32_t num_hexes = d.quot + d.remainder > 0;

        journey_route_t route;
        route.resize(num_hexes, hex);

        return route;
    }


    uint32_t hours_required(route_segment_t const& segment)
    {
        //segment.dist
    }
    

    journal_t make_long_journey(journey_route_t const& route, hex_database_t const& encounter_table_data)
    {
        std::vector<generated_hex_t> generated_route;
        generated_route.reserve(route.size());

        size_t days_spent_travelling = 0;
        size_t current_segment = 0;
        size_t current_hex_index = 0;

        size_t distance_remaining = 0;
        for(auto const& segment : route)
            distance_remaining += segment.dist;

        while(distance_remaining > 0)
        {
            auto const& cur_hex = generated_route[current_hex_index];

            //figure out how long it would take to travel the current segment
            travel_rate = travel_rate();            


            //if there's enough time left, travel the segment
            //else split the segment


            //travel segment
        }


        //journal_t journal;

        

    }




    std::string summarize(generated_hex_t const& hex)
    {
        std::string summary;

        summary += to_string(hex.coord) + ":";

        if(is_boring(hex))
        {
            summary += " nothing interesting";
        }
    }

    std::string summarize(journal_entry_t const& entry)
    {
        std::string summary;

        summary += to_string(entry.hexes.size()) + " hexes traveled\n";

        for(auto const& hex : entry.hexes)
        {
            summary += summarize(hex) + "\n";
        }
    }

    std::string summarize(long_journey_results_t const& journal)
    {
        std::string summary;

        summary += "Journey Stats:\n";
        summary += "    The Journey took " + to_string(journal.size()) + " days";

        size_t day_index = 0;

        for(auto const& entry : journal)
        {
            summary += "Day " + to_string(day_index + 1) + ":"
                summary += summarize(entry);
        }
    }


    int main(int argc, char*[] argv)
    {
        hex_database_t map_data;

        //TODO: take this as an argument?
        map_data.hex_generation_data = node_from_json(hex_encounter_data_filepath);

        //TODO: load route from file
        journey_route_t route = build_route(miles_per_hex * 10, hex_t()); //test route

        journal_t results = make_long_journey(route, map_data);
        cout << summarize(journal);
    }
}