#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch/catch.hpp>

#include "plant_generator.h"
#include "from_json.h"

using namespace std;
using namespace plantgen;

TEST_CASE("Variant Stuff")
{
    SECTION("Variant Comparators")
    {
        SECTION("Value Comparators")
        {
            std::string std_string = "string";
            variant_value_t variant_string = "string";

            REQUIRE(variant_string == std_string);
            REQUIRE(std_string == variant_string);

            std::string other_str = "something else entirely";
            variant_value_t other_variant_str = "something_else_entirely";

            REQUIRE(std_string != other_str);
            REQUIRE(variant_string != other_variant_str);
            
            REQUIRE(variant_string != other_str);
            REQUIRE(other_str != variant_string);

            REQUIRE(other_variant_str != std_string);
            REQUIRE(std_string != other_variant_str);


            auto cstr = "cstr";
            bool asdfasdf = variant_string == cstr;

            REQUIRE(variant_string == "string");
            REQUIRE("string" == variant_string);

            REQUIRE(variant_string != "not a reasonable string");
            REQUIRE("not a reasonable string" != variant_string);
        }
        
        /*SECTION("Value List Comparators")
        {
            std::vector<std::string> string_list = {"A", "B", "C", "D"};
            value_list_t variant_list = {"A", "B", "C", "D"};

            CHECK(string_list == variant_list);
            CHECK(variant_list == string_list);

            auto auto_strings = {"A","B","C","D"};
            REQUIRE(variant_list == auto_strings);
            REQUIRE(auto_strings == variant_list);

            std::vector<std::string> wrong_strings = {"X", "Y", "Z", "W"};
            REQUIRE(variant_list != wrong_strings);
            REQUIRE(wrong_strings != variant_list);
        }*/
    }
}