#include "fast_travel_sim.h"

#include <plantgen/plant_generator.h>
#include <plantgen/plant_printer.h>
#include <plantgen/from_json.h>

using namespace std;
using namespace plantgen;
using namespace plant_printer;


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
        auto modded_hex_dist = difficulty_modified_dist(route_seg.dist, hex_db.hex_at(route_seg.coord));

        return modded_hex_dist;
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



    bool is_boring(generated_node_t const& n)
    {
        return n.children.empty()
            && n.generated_values.empty()
            && n.value_nodes.empty();
    }

    bool is_boring(generated_hex_t const& h)
    {
        return is_boring(h.location)
            && is_boring(h.plant);
    }

    bool is_boring(journal_entry_t const& e)
    {
        if(e.segments.empty())
            return true;

        if(e.creatures.size() > 0)
            return false;

        for(auto const& jseg : e.segments)
        {
            for(auto const& loc : jseg.locations)
                if(!is_boring(loc))
                    return false;

            for(auto const& plant : jseg.plants)
                if(!is_boring(plant))
                    return false;
        }

        return true;
    }


    /// super verbose names
    constexpr double status_effect_weight_value_mult = 1000;
    constexpr double status_effect_subcategory_weight_value_mult = 500;
    constexpr double status_effect_duration_mult = 2.0;
    constexpr double status_effect_severity_mult = 5.0;

    int32_t status_effect_worth(generated_node_t const& status_effect)
    {
        ASSERT(status_effect.children.size() == 3, "");
        auto const& effect = status_effect.children[0].value_nodes[0];
        auto const& duration = status_effect.children[1];
        auto const& severity = status_effect.children[1];

        // start with something inversely proportional to the effect's weight
        double value = 1.0 / double(effect.weight) 
                     * status_effect_weight_value_mult;

        // if the effect has a sub-category, something inversely proportional to its weight
        if(effect.value_nodes.size() > 0)
        {
            value += 1.0 / double(effect.value_nodes[0].weight)
                   * status_effect_subcategory_weight_value_mult;
        }

        ASSERT(duration.value_index != nullindex, "duration index is null");
        ASSERT(severity.value_index != nullindex, "severity index is null");

        // value multipliers for duration and severity are based on
        // value_index as well as rarity
        // ie: rarity only matters if the value index is high

        {
            double percentage = double(duration.value_index) / double(duration.num_rollable_values_and_vnodes());
            double weight_mult = (1.0 / duration.weight);
            value *= 1.0 + (weight_mult * percentage) * status_effect_duration_mult;
        }
        {
            double percentage = double(severity.value_index) / double(severity.num_rollable_values_and_vnodes());
            double weight_mult = (1.0 / severity.weight);
            value *= 1.0 + (weight_mult * percentage) * status_effect_severity_mult;
        }

        return static_cast<int>(value); //truncate
    }

    int32_t plant_worth(generated_node_t const& plant)
    {
        return status_effect_worth(plant.children[1]);
    }


    void travel_group_t::add_traveller(traveller_t const& t)
    {
        adjust_cache(t);
        travellers.push_back(t);
    }

    void travel_group_t::add_traveller(traveller_t&& t)
    {
        adjust_cache(t);
        travellers.push_back(std::move(t));
    }

    void travel_group_t::adjust_cache(traveller_t const& t)
    {
        slowest_travel_rate      = std::min(slowest_travel_rate,      t.travel_rate);
        highest_survival_skill   = std::max(highest_survival_skill,   t.survival_skill);
        highest_navigation_skill = std::max(highest_navigation_skill, t.navigation_skill);
    }

    void travel_group_t::rebuild_cache()
    {
        slowest_travel_rate      = std::numeric_limits<int32_t>::max();
        highest_survival_skill   = std::numeric_limits<int32_t>::min();
        highest_navigation_skill = std::numeric_limits<int32_t>::min();

        for(auto const& t : travellers)
        {
            adjust_cache(t);
        }
    }

    /// WIP just doing 3d6 for now
    int32_t roll_dice()
    {
        int a = random_int(1, 6);
        int b = random_int(1, 6);
        int c = random_int(1, 6);

        return a+b+c;
    }

    bool test_skill(int32_t skill_rank)
    {
        return roll_dice() <= skill_rank;
    }
    

    /*
    for each segment
      travel as far as possible in the time remaining for the day
        split the segment if only part of it is travelled
      if the travel budget is empty
        add a journal entry for the day
    */

    //take journey by copy, as it will probably get modified later
    journal_t simulate_journey(journey_t journey, hex_database_t const& hex_db)
    {
        ASSERT(journey.travel_group.travellers.size() > 0, "Can't simulate a journey without travellers");
        WARN_IF(journey.route.empty(), "Simulating a journey without a route");

        auto& route = journey.route;

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
                route_segment_t new_seg(cur_seg);
                new_seg.dist = actual_untravelled;
                route.insert(route.begin() + current_seg_index + 1, std::move(new_seg));
            }


            // 'make camp' if there's no more travel remaining for the day
            //          or if this is the last segment
            if(daily_dist_remaining == 0 || current_seg_index + 1 == route.size())
            {
                //todo: encounters for the day
                auto creature_gen_node = generate_node(hex_db.creature_rollables);
                current_journal_entry.creatures.push_back(std::move(creature_gen_node));

                journey.day_rations -= journey.num_travellers();

                if(cur_seg.pace == travel_pace_foraging)
                {
                    for(auto const& traveller : journey.travel_group.travellers)
                    {
                        auto margin = -1 * roll_dice() - traveller.survival_skill;
                        margin = std::max(margin,0);

                        //1 for success, and another 1 for every 4 margins of success
                        journey.day_rations += (margin >= 0) + (margin / 4);
                    }
                }

                current_journal_entry.rations_remaining = journey.day_rations;
                journal.push_back(std::move(current_journal_entry));

                ++days_spent_travelling;
                daily_dist_remaining = num_hours_travelling * default_travel_rate;
            }
        }


        return journal;
    }

    std::string& indent_in_place(std::string& str, std::string indent_str)
    {
        if(str.empty())
            return str;

        //indent the opening line
        str.insert(str.begin(), indent_str.begin(), indent_str.end());

        size_t i = 0;
        while((i = str.find('\n', i)) != std::string::npos)
        {
            str.insert(str.begin() + i+1, indent_str.begin(), indent_str.end());
            ++i; //increment so I don't just find the same newline
        }

        return str;
    }



    void sort_by_worth(std::vector<plant_encounter_t>& plants)//, bool highest_first = true)
    {
        // int32_t sort_order_mult = (-2 * (-highest_first)) + 1; // +1 if true; -1 if false

        std::sort(plants.begin(), plants.end(), 
            [](plant_encounter_t const& lhs, plant_encounter_t const& rhs)
            {
                return plant_worth(lhs) > plant_worth(rhs);
            });
    }

    void sort_and_cull_unworthy_plants(std::vector<plant_encounter_t>& plants, int32_t cull_below_worth)
    {
        sort_by_worth(plants);

        for(size_t i = 0; i < plants.size(); ++i)
        {
            if(plant_worth(plants[i]) >= cull_below_worth)
            {
                plants.erase(plants.begin(), plants.begin() + i);
                return;
            }
        }
    }


    std::string summarize(creature_encounter_t const& enc)
    {
        if(enc.generated_values.size() > 0)
            return enc.generated_values[0];
        else if(enc.value_nodes.size() > 0)
            return print_plant(enc.value_nodes[0]);

        return "";
    }

    std::string summarize(location_encounter_t const& loc)
    {
        return loc.generated_values[0]
            +  " (" + loc.children[0].generated_values[0] + ")";
    }

    std::string summarize(plant_encounter_t const& plant)
    {
        auto plant_desc = value_name(plant.child("Physical Description"));
        string plant_worth_str = "$" + std::to_string(plant_worth(plant));

        return plant_worth_str + " :: " + plant_desc + ": " + print_sub_property(plant.child("Status Effect"));
    }

    std::string summarize(journey_segment_t const& seg)
    {
        std::string summary;

        auto pace_str = std::string(travel_pace_strings[seg.pace]);
        summary += "  " + to_string(seg.coord) + " (" + std::to_string(seg.dist) + " " + dist_units_name
                        + " at a " + pace_str + " pace)";

        for(auto const& loc : seg.locations)
        {
            summary += "\n    " + summarize(loc);
        }
        for(auto const& plant : seg.plants)
        {
            summary += "\n    " + summarize(plant);
        }

        return summary;
    }

    std::string summarize(journal_entry_t const& entry)
    {
        std::string summary;

        auto num_segs = convert_integer<size_t, uint64_t>(entry.segments.size());
        summary += std::to_string(num_segs) + " segments traveled; Rations Remaining: " + std::to_string(entry.rations_remaining) + "\n";

        for(auto const& seg : entry.segments)
        {
            summary += summarize(seg) + "\n";
        }
        for(auto const& enc : entry.creatures)
        {
            auto enc_sum = summarize(enc);
            indent_in_place(enc_sum, "    ");
            summary += enc_sum + "\n";
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
    route_segment_t{hex_coord_t{0,0}, path_paved,     travel_pace_normal,   miles_per_hex},
    route_segment_t{hex_coord_t{1,0}, path_paved,     travel_pace_normal,   miles_per_hex},
    route_segment_t{hex_coord_t{2,0}, path_dirt,      travel_pace_foraging, miles_per_hex},
    route_segment_t{hex_coord_t{3,0}, path_dirt,      travel_pace_foraging, miles_per_hex},
    route_segment_t{hex_coord_t{4,0}, path_none,      travel_pace_foraging, miles_per_hex},
    route_segment_t{hex_coord_t{5,0}, path_forest,    travel_pace_cautious, miles_per_hex},
    route_segment_t{hex_coord_t{6,0}, path_forest,    travel_pace_cautious, miles_per_hex},
    route_segment_t{hex_coord_t{7,0}, path_difficult, travel_pace_normal,   miles_per_hex}
};

const traveller_t test_traveller{"test",default_travel_rate,10,10};

constexpr int32_t test_rations = 100;


int main(int argc, char* argv[])
{
    hex_database_t hex_db;

    //TODO: take this as an argument?
    hex_db.hex_rollables = node_from_json(hex_encounter_data_filepath);
    hex_db.creature_rollables = node_from_json(creature_encounter_data_filepath);

    //TODO: load route from file
    journey_t journey;
    journey.travel_group.add_traveller(test_traveller);
    journey.route.insert(journey.route.begin(), test_route.begin(), test_route.end());
    journey.day_rations = test_rations;

    for(auto const& hex : test_hexes)
    {
        hex_db.generate_if_empty(hex);
    }

    journal_t results = simulate_journey(journey, hex_db);
    cout << summarize(results);

    return 0;
}