#include <catch/catch.hpp>

#include "plant_generator.h"
#include "from_json.h"

#include "nodes.h"

using namespace std;
using namespace plantgen;

TEST_CASE("Include")
{
    pregen_node_t node = node_from_json("../data/include.json");
    REQUIRE(node.children.size() == 5);
    REQUIRE(node.value_nodes.size() == 5);

    SECTION("Include As Property")
    {
        test_basic_node(node.children[0]);
        test_basic_node(node.children[1]);
        test_basic_node(node.children[2]);
        
        SECTION("With User Data")
        {
            auto const& n = node.children[3];

            REQUIRE(n.name == "Basic Node");
            REQUIRE(n.sub_name == "");
            REQUIRE(n.children.size() == 1);
            REQUIRE(n.value_nodes.size() == 4);
            REQUIRE(n.has_user_data());

            auto const& ud = n.user_data;
            CHECK(ud.children[0].name == "Comment");
            CHECK(ud.children[0].value_as<string>() == "Testing with User Data");
        }

        SECTION("Different Name")
        {
            auto const& n = node.children[4];

            REQUIRE(n.name == "## Basic Node ##");
            REQUIRE(n.sub_name == "Basic Node");
            REQUIRE(n.children.size() == 1); //properties
            REQUIRE(n.value_nodes.size() == 4);
            REQUIRE(!n.has_user_data());
        }
    }

    SECTION("Include As Value")
    {
        test_basic_node(node.value_nodes[0]);
        test_basic_node(node.value_nodes[1]);

        /// Lazy Copypaste
        SECTION("Different Name")
        {
            auto const& n = node.value_nodes[2];

            REQUIRE(n.name == "## Basic Node ##");
            REQUIRE(n.sub_name == "Basic Node");
            REQUIRE(n.children.size() == 1); //properties
            REQUIRE(n.value_nodes.size() == 4);
            REQUIRE(!n.has_user_data());
        }

        /// Lazy Copypaste, minor changes
        SECTION("With User Data And Different Name")
        {
            auto const& n = node.value_nodes[3];

            REQUIRE(n.name == "## Basic Node ##");
            REQUIRE(n.sub_name == "Basic Node");
            REQUIRE(n.children.size() == 1);
            REQUIRE(n.value_nodes.size() == 4);
            REQUIRE(n.has_user_data());

            auto const& ud = n.user_data;
            CHECK(ud.children[0].name == "Comment");
            CHECK(ud.children[0].value_as<string>() == "Testing with User Data");
        }

        SECTION("Include in VNode")
        {
            auto const& vn = node.value_nodes[4];

            REQUIRE(vn.name == "Include in VNode");
            REQUIRE(vn.sub_name == "");

            REQUIRE(vn.children.size() == 1);
            REQUIRE(vn.value_nodes.size() == 1);

            test_basic_node(vn.children[0]);
            test_basic_node(vn.value_nodes[0]);
        }
    }
}