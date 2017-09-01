#include <catch/catch.hpp>

#include "plant_generator.h"
#include "from_json.h"

#include "nodes.h"

using namespace std;
using namespace plantgen;

TEST_CASE("Merge")
{
    pregen_node_t node = node_from_json("../data/merge.json");
    // REQUIRE(node.children.size() == 5);
    // REQUIRE(node.value_nodes.size() == 9);

    SECTION("Merge {A, B}")
    {
        auto const& m = node.value_nodes[0];

        CHECK(m.name == "A");
        CHECK(m.sub_name == "");

        REQUIRE(m.children.size() == 4);
        CHECK(m.children[0].value == std::string("A P1"));
        CHECK(m.children[1].value == std::string("A P2"));
        CHECK(m.children[2].value == std::string("B P1"));
        CHECK(m.children[3].value == std::string("B P2"));

        REQUIRE(m.value_nodes.size() == 4);
        CHECK(m.value_nodes[0].value == std::string("A V1"));
        CHECK(m.value_nodes[1].value == std::string("A V2"));
        CHECK(m.value_nodes[2].value == std::string("B V1"));
        CHECK(m.value_nodes[3].value == std::string("B V2"));
    }

    SECTION("Merge {B, A}")
    {
        auto const& m = node.value_nodes[1];

        CHECK(m.name == "B");
        CHECK(m.sub_name == "");

        REQUIRE(m.children.size() == 4);
        CHECK(m.children[0].value == std::string("B P1"));
        CHECK(m.children[1].value == std::string("B P2"));
        CHECK(m.children[2].value == std::string("A P1"));
        CHECK(m.children[3].value == std::string("A P2"));

        REQUIRE(m.value_nodes.size() == 4);
        CHECK(m.value_nodes[0].value == std::string("B V1"));
        CHECK(m.value_nodes[1].value == std::string("B V2"));
        CHECK(m.value_nodes[2].value == std::string("A V1"));
        CHECK(m.value_nodes[3].value == std::string("A V2"));
        
    }

    SECTION("Merge {Include, Value Node}")
    {
        auto const& m = node.value_nodes[2];

        CHECK(m.name == "Basic Node");
        CHECK(m.sub_name == "");

        REQUIRE(m.children.size() == 3);

        auto const& bn_p = m.children[0];
        REQUIRE(bn_p.name == std::string("Property"));
        CHECK(bn_p.value_nodes[0].value == std::string("1 A"));
        CHECK(bn_p.value_nodes[1].value == std::string("1 B"));
        CHECK(bn_p.value_nodes[2].value == std::string("1 C"));
        CHECK(bn_p.value_nodes[3].value == std::string("1 D"));

        CHECK(m.children[1].value == std::string("Merge Test P1"));
        CHECK(m.children[2].value == std::string("Merge Test P2"));

        REQUIRE(m.value_nodes.size() == 6);
        CHECK(m.value_nodes[0].value == std::string("A"));
        CHECK(m.value_nodes[1].value == std::string("B"));
        CHECK(m.value_nodes[2].value == std::string("C"));
        CHECK(m.value_nodes[3].value == std::string("D"));
        CHECK(m.value_nodes[4].value == std::string("Merge Test V1"));
        CHECK(m.value_nodes[5].value == std::string("Merge Test V2"));
    }

    SECTION("Merge {Value Node, Include}")
    {
        auto const& m = node.value_nodes[3];

        CHECK(m.name == "MergeTestNode");
        CHECK(m.sub_name == "");

        REQUIRE(m.children.size() == 3);
        CHECK(m.children[0].value == std::string("Merge Test P1"));
        CHECK(m.children[1].value == std::string("Merge Test P2"));

        auto const& bn_p = m.children[2];
        REQUIRE(bn_p.name == std::string("Property"));
        CHECK(bn_p.value_nodes[0].value == std::string("1 A"));
        CHECK(bn_p.value_nodes[1].value == std::string("1 B"));
        CHECK(bn_p.value_nodes[2].value == std::string("1 C"));
        CHECK(bn_p.value_nodes[3].value == std::string("1 D"));

        REQUIRE(m.value_nodes.size() == 6);
        CHECK(m.value_nodes[0].value == std::string("Merge Test V1"));
        CHECK(m.value_nodes[1].value == std::string("Merge Test V2"));
        CHECK(m.value_nodes[2].value == std::string("A"));
        CHECK(m.value_nodes[3].value == std::string("B"));
        CHECK(m.value_nodes[4].value == std::string("C"));
        CHECK(m.value_nodes[5].value == std::string("D"));
    }


    SECTION("Error Handling")
    {
        SECTION("Invalid JSON Type")
        {
            cJSON* json_root = cJSON_Parse(
            R"~({
                "Merge":[
                    9001,
                    {
                        "Name":"Valid Node"
                    }
                ]
            })~");

            REQUIRE_THROWS_AS(node_from_json(json_root), json_type_exception const&);
        }

        SECTION("Failed Include")
        {
            SECTION("String Include")
            {
                cJSON* json_root = cJSON_Parse(
                R"~({
                    "Merge":[
                        "not_a_real_file.some_ext",
                        {
                            "Name":"Valid Node"
                        }
                    ]
                })~");

                REQUIRE_THROWS_AS(node_from_json(json_root), include_not_found_exception const&);
            }

            SECTION("Node Include")
            {
                cJSON* json_root = cJSON_Parse(
                R"~({
                    "Merge":[
                        {
                            "Include":"not_a_real_file.some_ext"
                        },
                        {
                            "Name":"Valid Node"
                        }
                    ]
                })~");

                REQUIRE_THROWS_AS(node_from_json(json_root), include_not_found_exception const&);
            }
        }
    }
}