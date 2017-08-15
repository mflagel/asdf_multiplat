#include <catch/catch.hpp>

#include "plant_generator.h"
#include "from_json.h"

using namespace std;
using namespace plantgen;

void test_basic_node(pregen_node_t const& node)
{
    REQUIRE(node.name == "Basic Node");
    REQUIRE(node.sub_name == "");
    REQUIRE(node.children.size() == 1); //properties
    REQUIRE(node.value_nodes.size() == 4);
    REQUIRE(!node.has_user_data());

    REQUIRE(node.children[0].name == "Property");
    REQUIRE(node.children[0].value_nodes.size() == 4);

    std::vector<std::string> expected_node_value_strings = {"A", "B", "C", "D"};
    for(size_t i = 0; i < node.value_nodes.size(); ++i)
    {
        CHECK( node.value_nodes[i].name.empty() );
        CHECK( node.value_nodes[i].value == expected_node_value_strings[i] );
    }


    expected_node_value_strings = {"1 A", "1 B", "1 C", "1 D"};
    auto const& c_vns = node.children[0].value_nodes;
    for(size_t i = 0; i < c_vns.size(); ++i)
    {
        CHECK( node.value_nodes[i].name.empty() );
        CHECK( c_vns[i].value == expected_node_value_strings[i] );
    }
}

TEST_CASE("Node Stuff")
{
    SECTION("Basic Node")
    {
        test_basic_node(node_from_json("../data/basic_node.json"));
        test_basic_node(node_from_json("../data/basic_node_extended.json"));
        
    }
    
    SECTION("Fancy Node")
    {
        pregen_node_t node = node_from_json("../data/fancy_node.json");
        
        REQUIRE(node.name == "Fancy Node");
        REQUIRE(node.children.size() == 2);
        REQUIRE(node.value_nodes.size() == 4);

        auto const& p1 = node.children[0];
        REQUIRE(p1.name == "Property One");
        REQUIRE(p1.children.size() == 0);
        REQUIRE(p1.value_nodes.size() == 4);

        auto const& p2 = node.children[1];
        REQUIRE(p2.name == "Property Two");
        REQUIRE(p2.children.size() == 2);
        REQUIRE(p2.value_nodes.size() == 0);

        auto const& p2a = p2.children[0];
        REQUIRE(p2a.name == "Property Two Alpha");
        REQUIRE(p2a.value_nodes.size() == 4);
        REQUIRE(p2a.children.size() == 0);

        auto const& p2b = p2.children[1];
        REQUIRE(p2b.name == "Property Two Beta");
        REQUIRE(p2b.children.size() == 1);
        REQUIRE(p2b.value_nodes.size() == 0);

        auto const& p2b_1 = p2b.children[0];
        REQUIRE(p2b_1.name == "Property Two Beta Sub1");
        REQUIRE(p2b_1.children.size() == 0);
        REQUIRE(p2b_1.value_nodes.size() == 1);


        /// children values (and grandchildren values... etc)
        /// p1
        CHECK(p1.value_nodes[0].value == "1 A");
        CHECK(p1.value_nodes[1].value == "1 B");
        CHECK(p1.value_nodes[2].name == "ValueObj 1 A");
        CHECK(p1.value_nodes[3].name == "ValueObj 1 B");
        
        /// p2a
        std::array<const char* const, 4> p2a_value_strings = {"2A - A", "2A - B", "2A - C", "2A - D"};
        for(size_t i = 0; i < p2a.value_nodes.size(); ++i)
        {
            CHECK(p2a.value_nodes[i].value == p2a_value_strings[i]);
        }

        ///p2b_1
        auto const& p2b_1_vnode = p2b_1.value_nodes[0];
        CHECK(p2b_1_vnode.name == "P2B_1 Value Node A");
        CHECK(p2b_1_vnode.value_nodes.size() == 4);
        std::array<const char* const, 4> p2b_1_vnode_value_strings = {"Value Node Value A", "Value Node Value B", "Value Node Value C", "Value Node Value D"};
        for(size_t i = 0; i < p2b_1_vnode.value_nodes.size(); ++i)
        {
            CHECK(p2b_1_vnode.value_nodes[i].value == p2b_1_vnode_value_strings[i]);
        }


        /// root node values
        CHECK(node.value_nodes[0].value == "ValueString A");
        CHECK(node.value_nodes[1].value == "ValueString B");
        CHECK(std::holds_alternative<null_value_t>(node.value_nodes[2].value));

        // CHECK(node.value_nodes[2].value == "");
        CHECK(node.value_nodes[3].value == "ValueString D");

        auto const& vn = node.value_nodes[2];
        CHECK(vn.name == "ValueObj A");
        REQUIRE(vn.children.size() == 1);
        REQUIRE(vn.value_nodes.size() == 5);

        auto const& vn_p1 = vn.children[0];
        REQUIRE(vn_p1.name == "ValueObj Property One");
        std::array<const char* const, 4> vn_p1_value_strings = {"VObj P1 A", "VObj P1 B", "VObj P1 C", "VObj P1 D"};
        for(size_t i = 0; i < vn_p1_value_strings.size(); ++i)
        {
            CHECK(vn_p1.value_nodes[i].value == vn_p1_value_strings[i]);
        }

        CHECK(vn.value_nodes[0].name == "Value Obj Value Obj");
        std::array<const char* const, 4> vn_value_strings = {"ValueObj Value A", "ValueObj Value B", "ValueObj Value C", "ValueObj Value D"};
        for(size_t i = 0; i < vn_value_strings.size(); ++i)
        {
            CHECK(vn.value_nodes[i+1].value == vn_value_strings[i]);
        }
    }
}