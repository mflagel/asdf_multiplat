#include <catch/catch.hpp>

#include "plant_generator.h"
#include "from_json.h"

using namespace std;
using namespace plantgen;


TEST_CASE("Weights")
{
    pregen_node_t node = node_from_json("../data/weights.json");

    REQUIRE(node.name == "Weights");
    REQUIRE(node.children.size() == 4);
    REQUIRE(node.value_nodes.size() == 8);

    CHECK(node.value_nodes[0].weight == 100);
    CHECK(node.value_at(0) == "A");
    CHECK(node.value_nodes[1].weight == 20);
    CHECK(node.value_at(1) == "B");
    CHECK(node.value_nodes[2].weight == 1);
    CHECK(node.value_at(2) == "C");
    CHECK(node.value_nodes[3].weight == 1);
    CHECK(node.value_at(3) == "D");
    CHECK(node.value_nodes[4].weight == 0);
    CHECK(node.value_at(4) == "E");

    CHECK(node.value_nodes[5].name == "ValueNode A");
    CHECK(node.value_nodes[5].weight == 25);
    CHECK(node.value_nodes[6].name == "ValueNode B");
    CHECK(node.value_nodes[6].weight == 250);
    CHECK(node.value_nodes[7].name == "ValueNode C");
    CHECK(node.value_nodes[7].weight == 4);
    CHECK(node.value_nodes[7].value_nodes.size() == 4);

    SECTION("Normal Weights")
    {
        auto const& n = node.children[0];
        REQUIRE(n.name == "Normal Weights");
        REQUIRE(n.value_nodes.size() == 3);

        CHECK(n.value_nodes[0].name == "Normal A");
        CHECK(n.value_nodes[0].weight == 123);
        CHECK(n.value_nodes[1].name == "Normal B");
        CHECK(n.value_nodes[1].weight == 456);
        CHECK(n.value_nodes[2].name == "Normal C");
        CHECK(n.value_nodes[2].weight == 789);
    }

    SECTION("Inline Weights")
    {
        auto const& n = node.children[1];
        REQUIRE(n.name == "Inline Weights");
        REQUIRE(n.value_nodes.size() == 3);

        CHECK(n.value_at(0) == "Inline A");
        CHECK(n.value_nodes[0].weight == 123);
        CHECK(n.value_at(1) == "Inline B");
        CHECK(n.value_nodes[1].weight == 456);
        CHECK(n.value_at(2) == "Inline C");
        CHECK(n.value_nodes[2].weight == 789);
    }

    SECTION("Inline Normal Mix")
    {
        auto const& n = node.children[2];
        REQUIRE(n.name == "Inline Normal Mix");
        REQUIRE(n.value_nodes.size() == 6);

        //lazy copypaste
        CHECK(n.value_at(0) == "Inline A");
        CHECK(n.value_nodes[0].weight == 123);
        CHECK(n.value_at(1) == "Inline B");
        CHECK(n.value_nodes[1].weight == 456);
        CHECK(n.value_at(2) == "Inline C");
        CHECK(n.value_nodes[2].weight == 789);

        //lazy copypaste
        CHECK(n.value_nodes[3].name == "Normal A");
        CHECK(n.value_nodes[3].weight == 123);
        CHECK(n.value_nodes[4].name == "Normal B");
        CHECK(n.value_nodes[4].weight == 456);
        CHECK(n.value_nodes[5].name == "Normal C");
        CHECK(n.value_nodes[5].weight == 789);
    }

    SECTION("Weight of Values")
    {
        auto const& wov = node.children[3];
        REQUIRE(wov.name == "Weight of Values");
        REQUIRE(wov.value_nodes.size() == 1);

        auto const& wov_vn = wov.value_nodes[0];
        auto sum = total_weight(wov_vn.value_nodes);
        REQUIRE(wov_vn.weight == 1464); //123 + 456 + 789 + 32 + 64
        REQUIRE(wov_vn.weight == sum);
    }
}