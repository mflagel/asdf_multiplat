#include <catch/catch.hpp>

#include "plant_generator.h"
#include "from_json.h"

using namespace std;
using namespace plantgen;

TEST_CASE("User Data")
{
    pregen_node_t node = node_from_json("../data/user_data.json");

    REQUIRE(node.name == "User Data");
    REQUIRE(node.children.size() == 4);
    REQUIRE(node.value_nodes.size() == 0);


    auto check_array = [](auto* arr_n, auto&& vals)
    {
        REQUIRE(arr_n);
        REQUIRE(arr_n->children.size() == vals.size());

        if(arr_n->children.size() == 0)
            return;

        using T = typename std::decay_t<decltype(vals[0])>;

        for(size_t i = 0; i < vals.size(); ++i)
        {
            auto const& arr_v = std::get<T>(arr_n->children[i].value);

            if constexpr(std::is_same<T, double>::value)
            {
                double diff = arr_v - vals[i];
                //std::numeric_limits<double>::epsilon was giving me 0.0
                //but the diff ends up being non-zero (probably due to how
                //cJSON loads floating point numbers)
                CHECK( abs(diff) < 0.0000000001);
            }
            else
            {
                CHECK(arr_v == vals[i]);
            }
        }
    };

    auto check_keywords = [](user_data_node_t const& ud)
    {
        CHECK(find(ud, "Name") == nullptr);
        CHECK(find(ud, "Properties") == nullptr);
        CHECK(find(ud, "Values") == nullptr);
        CHECK(find(ud, "Weight") == nullptr);
        CHECK(find(ud, "Include") == nullptr);
    };


    SECTION("Bool Test")
    {
        auto const& bt_n = node.children[0];
        REQUIRE(bt_n.name == "Bool Test");
        REQUIRE(bt_n.children.size() == 0);
        REQUIRE(bt_n.value_nodes.size() == 4);
        
        auto const& ud = bt_n.user_data;
        check_keywords(ud);

        REQUIRE(ud.children.size() == 3);
        CHECK(find(ud, "TrueTest")->value_as<bool>() == true);
        CHECK(find(ud, "FalseTest")->value_as<bool>() == false);

        auto* bool_array_node = find(ud, "Bool Array");
        REQUIRE(bool_array_node);
        std::array<bool, 6> bool_array_vals = {true, false, false, true, true, false};
        REQUIRE(bool_array_node->children.size() == bool_array_vals.size());
        for(size_t i = 0; i < bool_array_node->children.size(); ++i)
        {
            CHECK(bool_array_node->children[i].value_as<bool>() == bool_array_vals[i]);
        }
    }

    SECTION("Number Test")
    {
        auto const& n_n = node.children[1];
        REQUIRE(n_n.name == "Number Test");
        REQUIRE(n_n.children.size() == 0);
        REQUIRE(n_n.value_nodes.size() == 4);
        CHECK(n_n.weight == 9001);
        
        auto const& ud = n_n.user_data;
        check_keywords(ud);

        CHECK(find(ud, "Integer 1")->value_as<double>() == 5);
        CHECK(find(ud, "Integer 2")->value_as<double>() == 3463456);
        CHECK(find(ud, "Double 1")->value_as<double>() == 981209.687678);
        CHECK(find(ud, "Double 2")->value_as<double>() == 123456789.912987123);

        
        check_array(find(ud, "Integer Array"), std::vector<double>({
              1
            , 2
            , 3
            , 4
            , 5
            , 6
            , 7
            , 8
            , 9
            , 9001
            , 6182638
            , 973979238
            }));
        check_array(find(ud, "Double Array"), std::vector<double>({
              900.1
            , 9372598.52342
            , 12312.457567
            , 385893.988123
            }));
        check_array(find(ud, "Mixed Array"), std::vector<double>({
              123
            , 456
            , 789.123454
            , 1209313.46456
            , 9001
            , 84929487.0218218
            }));
    }

    SECTION("String Test")
    {
        auto const& s_n = node.children[2];
        REQUIRE(s_n.name == "String Test");
        REQUIRE(s_n.children.size() == 0);
        REQUIRE(s_n.value_nodes.size() == 0);

        
        auto const& ud = s_n.user_data;
        check_keywords(ud);

        CHECK(find(ud, "String 1")->value_as<string>() == std::string("Test String A"));
        CHECK(find(ud, "String 2")->value_as<string>() == std::string("Test String B"));
        CHECK(find(ud, "String 3")->value_as<string>() == std::string(""));

        
        check_array(find(ud, "String Array"), std::vector<std::string>({
             std::string("Array String A")
            ,std::string("Array String B")
            ,std::string("Array String C")
            ,std::string("Array String D")
            }));

        check_array(find(ud, "String Array 2"), std::vector<std::string>({
             "Array 2 A"
            ,"Array 2 B"
            ,""
            ,"Test"
            }));
    }
}