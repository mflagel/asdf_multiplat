#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch/catch.hpp>

#include "plant_generator.h"
#include "from_json.h"

using namespace std;
using namespace plantgen;

namespace
{
    constexpr size_t num_range_rng_tests = 1000;
}

TEST_CASE("Testing the Test Framework")
{
    SECTION("Variant Comparators")
    {
        SECTION("Value Comparators")
        {
            string std_string = "string";
            variant_value_t variant_string = "string";

            REQUIRE(variant_string == std_string);
            REQUIRE(std_string == variant_string);


            std::string other_str = "something else entirely";
            variant_value_t other_variant_str = "something_else_entirely";

            REQUIRE(std_string != other_str);
            REQUIRE(variant_string != other_variant_str);
            
            REQUIRE(variant_string != other_str);
            //REQUIRE(other_str != variant_string);

            REQUIRE(other_variant_str != std_string);
            //REQUIRE(std_string != other_variant_str);
        }

        SECTION("Value List Comparators")
        {
            std::vector<std::string> string_list = {"A", "B", "C", "D"};
            value_list_t variant_list = {"A", "B", "C", "D"};

            CHECK(string_list == variant_list);
            CHECK(variant_list == string_list);

            auto auto_strings = {"A","B","C","D"};
            REQUIRE(variant_list == auto_strings);

            std::vector<std::string> wrong_strings = {"X", "Y", "Z", "W"};
            REQUIRE(variant_list != wrong_strings);
        }
    }

    SECTION("Basic Node")
    {
        pregen_node_t node = node_from_json("data/basic_node.json");

        REQUIRE(node.name == "Basic Node");
        REQUIRE(node.values.size() == 4);
        REQUIRE(node.children.size() == 1); //properties

        REQUIRE(node.children[0].name == "Property");
        REQUIRE(node.children[0].values.size() == 4);

        std::vector<std::string> expected_node_value_strings = {"A", "B", "C", "D"};
        CHECK(node.values == expected_node_value_strings);


        expected_node_value_strings = {"1 A", "1 B", "1 C", "1 D"};
        CHECK(node.children[0].values == expected_node_value_strings);
    }

    SECTION("Fancy Node")
    {
        pregen_node_t node = node_from_json("data/fancy_node.json");

        REQUIRE(node.name == "Fancy Node");

        /// Properties
        REQUIRE(node.children.size() == 2);

        auto const& p1 = node.children[0];
        REQUIRE(p1.children.size() == 0);

        auto const& p2 = node.children[1];
        REQUIRE(p2.children.size() == 2);

        auto const& p2a = p2.children[0];
        auto const& p2b = p2.children[1];
        REQUIRE(p2a.children.size() == 0);
        REQUIRE(p2a.children.size() == 1);

        auto const& p2b_1 = p2b.children[0];
        REQUIRE(p2b_1.children.size() == 0);


        /// Values
        REQUIRE(node.values.size() == 2);
        REQUIRE(node.value_nodes.size() == 1);

        auto node_value_strings = {"ValueString A", "ValueString B"};
        CHECK(node.values == node_value_strings);

        auto const& vn = node.value_nodes[0];
        CHECK(vn.name == "ValueObj A");
        REQUIRE(vn.children.size() == 1);
        REQUIRE(vn.values.size() == 4);
        REQUIRE(vn.value_nodes.size() == 1);

        auto vn_value_strings = {"ValueObj Value A", "ValueObj Value B", "ValueObj Value C", "ValueObj Value D"};
        CHECK(vn.values == vn_value_strings);
        REQUIRE(vn.value_nodes[0].name == "Value Obj Value Obj");

        auto const& vn_p1 = vn.children[0];
        REQUIRE(vn_p1.name == "ValueObj Property One");
        auto vn_p1_value_strings = {"VObj P1 A", "VObj P1 B", "VObj P1 C", "VObj P1 D"};
        REQUIRE(vn_p1.values == vn_p1_value_strings);

        auto p1_value_strings = {"1 A", "1 B"};
        CHECK(p1.values == p1_value_strings);
        CHECK(p1.value_nodes[0].name == "ValueObj 1 A");
        CHECK(p1.value_nodes[1].name == "ValueObj 1 B");

        auto p2a_value_strings = {"2A - A", "2A - B", "2A - C", "2A - D"};
        CHECK(p2a.values == p2a_value_strings);

        REQUIRE(p2b_1.value_nodes.size() == 1);
        auto const& p2b_1_vnode = p2b_1.value_nodes[0];
        CHECK(p2b_1_vnode.name == "P2B_1 Value Node A");
        auto p2b_1_vnode_value_strings = {"Value Node Value A", "Value Node Value B", "Value Node Value C", "Value Node Value D"};
        REQUIRE(p2b_1_vnode.values == p2b_1_vnode_value_strings);
    }

    SECTION("Range Value")
    {
        pregen_node_t node = node_from_json("data/range_value.json");

        REQUIRE(node.name == "Range Value");

        REQUIRE(node.children.size() == 1);
        REQUIRE(node.values.size() == 2);
        REQUIRE(node.value_nodes.size() == 0);

        REQUIRE(std::holds_alternative<range_value_t>(node.values[0]));
        REQUIRE(std::holds_alternative<range_value_t>(node.values[1]));

        range_value_t r0 = std::get<range_value_t>(node.values[0]);
        std::vector<std::string> r0_strings = {"1 A", "1 B", "1 C", "1 D"};
        REQUIRE(r0 == r0_strings);

        range_value_t r1 = std::get<range_value_t>(node.values[1]);
        std::vector<std::string> r1_strings = {"2 A", "2 B", "2 C", "2 D"};
        REQUIRE(r1 == r1_strings);


        auto const& p1 = node.children[0];
        REQUIRE(p1.children.size() == 0);
        REQUIRE(p1.values.size() == 5);
        REQUIRE(p1.value_nodes.size() == 1);

        CHECK(p1.values[0] == std::string("NotRange A"));
        CHECK(p1.values[1] == std::string("NotRange B"));
        CHECK(p1.values[2] == std::string("NotRange C"));
        REQUIRE(std::holds_alternative<range_value_t>(p1.values[3]));
        REQUIRE(std::holds_alternative<range_value_t>(p1.values[4]));

        range_value_t p1r0 = std::get<range_value_t>(p1.values[3]);
        std::vector<std::string> p1r0_strings = {"P1R1 A", "P1R1 B", "P1R1 C", "P1R1 D"};
        REQUIRE(p1r0 == p1r0_strings);

        range_value_t p1r1 = std::get<range_value_t>(p1.values[4]);
        std::vector<std::string> p1r1_strings = {"P1R2 A", "P1R2 B", "P1R2 C", "P1R2 D"};
        REQUIRE(p1r1 == p1r1_strings);


        SECTION("Range Generation")
        {
            auto generated = generate_node(node);

            REQUIRE(generated.name == node.name);
            REQUIRE(generated.generated_values.size() == node.children.size());

            auto get_total = [](std::vector<std::string> const& gend_vals) -> uint32_t
            {
                uint32_t total = 0;
                for(auto const& v : gend_vals)
                {
                    total += stoi(v);
                }
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
        //TODO
    }

    /*
    SECTION("Weights")
    {
        pregen_node_t node = node_from_json("data/weights.json");

        REQUIRE(node.name == "Weights");

        REQUIRE(node.children.size() == 4);

        REQUIRE(node.values.size() == 5);
        CHECK(node.values[0].weight == 100);
        CHECK(node.values[0] == "A");
        CHECK(node.values[1].weight == 20);
        CHECK(node.values[1] == "B");
        CHECK(node.values[2].weight == 1);
        CHECK(node.values[2] == "C");
        CHECK(node.values[3].weight == 1);
        CHECK(node.values[3] == "D");
        CHECK(node.values[4].weight == 0);
        CHECK(node.values[4] == "E");

        CHECK(node.value_nodes[0].name == "ValueNode A");
        CHECK(node.value_nodes[0].weight == 25);
        CHECK(node.value_nodes[1].name == "ValueNode B");
        CHECK(node.value_nodes[1].weight == 250);
        CHECK(node.value_nodes[2].name == "ValueNode C");
        CHECK(node.value_nodes[2].weight == 4);
        CHECK(node.value_nodes[2].values.size() == 4);

        SECTION("Normal Weights")
        {
            auto const& n = node.children[0];
            REQUIRE(n.name == "Normal Weights");
            REQUIRE(n.values.size() == 0);
            REQUIRE(n.values_nodes.size() == 3);

            CHECK(n.values_nodes[0].name == "Normal A");
            CHECK(n.values_nodes[0].weight == 123)
            CHECK(n.values_nodes[1].name == "Normal B");
            CHECK(n.values_nodes[1].weight == 456)
            CHECK(n.values_nodes[2].name == "Normal C");
            CHECK(n.values_nodes[2].weight == 789)
        }

        SECTION("Inline Weights")
        {
            auto const& n = node.children[1];
            REQUIRE(n.name == "Inline Weights");
            REQUIRE(n.values.size() == 3);
            REQUIRE(n.values_nodes.size() == 0);

            CHECK(n.values_nodes[0].name == "Inline A");
            CHECK(n.values_nodes[0].weight == 123)
            CHECK(n.values_nodes[1].name == "Inline B");
            CHECK(n.values_nodes[1].weight == 456)
            CHECK(n.values_nodes[2].name == "Inline C");
            CHECK(n.values_nodes[2].weight == 789)
        }

        SECTION("Inline Normal Mix")
        {
            auto const& n = node.children[2];
            REQUIRE(n.name == "Inline Normal Mix");
            REQUIRE(n.values.size() == 3);
            REQUIRE(n.values_nodes.size() == 3);

            //lazy copypaste
            CHECK(n.values_nodes[0].name == "Normal A");
            CHECK(n.values_nodes[0].weight == 123)
            CHECK(n.values_nodes[1].name == "Normal B");
            CHECK(n.values_nodes[1].weight == 456)
            CHECK(n.values_nodes[2].name == "Normal C");
            CHECK(n.values_nodes[2].weight == 789)

            //lazy copypaste
            CHECK(n.values_nodes[0].name == "Inline A");
            CHECK(n.values_nodes[0].weight == 123)
            CHECK(n.values_nodes[1].name == "Inline B");
            CHECK(n.values_nodes[1].weight == 456)
            CHECK(n.values_nodes[2].name == "Inline C");
            CHECK(n.values_nodes[2].weight == 789)
        }

        SECTION("Weight of Values")
        {
            auto const& n = node.children[3];
            REQUIRE(n.name == "Weight of Values");
            REQUIRE(n.values.size() == 0);
            REQUIRE(n.values_nodes.size() == 1);

            auto const& vn = node.value_nodes[0];
            auto sum = total_weight(vn);
            REQUIRE(vn.weight == sum);
        }
    }
    */
}