#include <catch/catch.hpp>

#include "plant_generator.h"
#include "from_json.h"

using namespace std;
using namespace plantgen;

namespace
{
    constexpr size_t num_range_rng_tests = 1000;
}

TEST_CASE("Value Types")
{
    
    SECTION("Range Value")
    {

        pregen_node_t node = node_from_json("../data/range_value.json");

        REQUIRE(node.name == "Range Value");

        REQUIRE(node.children.size() == 1);
        REQUIRE(node.value_nodes.size() == 2);

        
        REQUIRE(std::holds_alternative<range_value_t>(node.value_nodes[0].value));
        REQUIRE(std::holds_alternative<range_value_t>(node.value_nodes[1].value));

        
        
        range_value_t r0 = std::get<range_value_t>(node.value_nodes[0].value);
        std::vector<std::string> r0_strings = {"1 A", "1 B", "1 C", "1 D"};
        REQUIRE(r0 == r0_strings);

        range_value_t r1 = std::get<range_value_t>(node.value_at(1));
        std::vector<std::string> r1_strings = {"2 A", "2 B", "2 C", "2 D"};
        REQUIRE(r1 == r1_strings);

        
        auto const& p1 = node.children[0];
        REQUIRE(p1.children.size() == 0);
        REQUIRE(p1.value_nodes.size() == 6);
        
        CHECK(p1.value_at(0) == std::string("NotRange A"));
        CHECK(p1.value_at(1) == std::string("NotRange B"));
        CHECK(p1.value_at(2) == std::string("NotRange C"));
        REQUIRE( std::holds_alternative<range_value_t>(p1.value_at(3)) );
        REQUIRE( std::holds_alternative<range_value_t>(p1.value_at(4)) );
        
        range_value_t p1r0 = std::get<range_value_t>(p1.value_at(3));
        std::vector<std::string> p1r0_strings = {"P1R1 A", "P1R1 B", "P1R1 C", "P1R1 D"};
        REQUIRE(p1r0 == p1r0_strings);

        range_value_t p1r1 = std::get<range_value_t>(p1.value_at(4));
        std::vector<std::string> p1r1_strings = {"P1R2 A", "P1R2 B", "P1R2 C", "P1R2 D"};
        REQUIRE(p1r1 == p1r1_strings);

        
        SECTION("Range Generation")
        {
            auto generated = generate_node(node);
            
            REQUIRE(generated.name == node.name);
            auto const& range_val = std::get<range_value_t>(node.value_nodes[0].value);
            REQUIRE(generated.generated_values.size() == range_val.size());
            
            auto get_total = [](std::vector<std::string> const& gend_vals) -> uint32_t
            {
                uint32_t total = 0;
                for(auto const& v : gend_vals)
                {
                    total += stoi(v);
                }

                return total;
            };
            
            REQUIRE(get_total(generated.generated_values) == 100);

            bool all_range_rng_passed = true;
            for(size_t i = 0; i < num_range_rng_tests; ++i)
                all_range_rng_passed &= (get_total(generated.generated_values) == 100);

            REQUIRE(all_range_rng_passed);
        }
    }
    
    

    SECTION("Multi Value")
    {
        pregen_node_t node = node_from_json("../data/multi_value.json");

        REQUIRE(node.name == "Multi Value");
        REQUIRE(node.children.size() == 1);
        REQUIRE(node.value_nodes.size() == 3);

        auto const& pn = node.children[0];
        auto const& p1_m = pn.value_nodes[0].value_as<multi_value_t>();
        CHECK(p1_m.num_to_pick == 3);
        CHECK(p1_m.values.size() == 4);

        auto const& vn_m1 = node.value_nodes[0].value_as<multi_value_t>();;
        CHECK(vn_m1.num_to_pick == 2);
        CHECK(vn_m1.values.size() == 4);

        auto const& vn_m2 = node.value_nodes[1].value_as<multi_value_t>();;
        CHECK(vn_m2.num_to_pick == 4);
        CHECK(vn_m2.values.size() == 4);


        auto check_roll = 
            [](multi_value_t const& mv){
                auto rolled = roll_multi_value(mv);
                CHECK(rolled.size() == mv.num_to_pick);

                //ensure each rolled value is unique
                for(size_t i = 0; i < rolled.size(); ++i)
                {
                    for(size_t j = 0; j < rolled.size(); ++j)
                    {
                        if(j != i)
                        {
                            CHECK(rolled[i].value != rolled[j].value);
                        }
                    }
                }
            };

        check_roll(p1_m);
        check_roll(vn_m1);
        check_roll(vn_m2);

        /// TODO check if after 1000 rolls, each value is touched at least once

        SECTION("Multi with VN nodes")
        {
            auto const& multi = node.value_nodes[0].value_as<multi_value_t>();

            CHECK(multi.num_to_pick == 2);
            CHECK(multi.values.size() == 4);
        }
    }
}