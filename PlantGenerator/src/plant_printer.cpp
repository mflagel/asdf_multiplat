#include "plant_printer.h"

#include <algorithm>

#include "asdf_multiplat/main/asdf_defs.h"

using namespace std;
using namespace plantgen;

namespace plant_printer
{
    struct sorted_flavor_t
    {
        int percentage;
        // std::string_view flavor_string;
        std::string flavor_string;
    };

    std::vector<sorted_flavor_t> get_sorted_flavors(generated_node_t const& node)
    {
        ASSERT(node.generated_values.size() > 0, "expected some flavors to sort");
        std::vector<sorted_flavor_t> sorted_flavors;
        sorted_flavors.reserve(node.generated_values.size());
        
        for(auto const& val : node.generated_values)
        {
            auto percent_sign_pos = val.find('%');

            ASSERT(percent_sign_pos != std::string::npos, "");
            if(percent_sign_pos != std::string::npos)
            {
                sorted_flavor_t sf =
                {
                    stoi(val.substr(0, percent_sign_pos)),
                    val.substr(percent_sign_pos)
                };

                sorted_flavors.push_back(sf);
            }
        }

        std::sort(sorted_flavors.begin(), sorted_flavors.end(),
            [](sorted_flavor_t const& lhs, sorted_flavor_t const& rhs)
            {
                return lhs.percentage > rhs.percentage;
            });

        return sorted_flavors;   
    }


    string print_simple_flavor(generated_node_t const& node);    

    string print_complex_flavor(generated_node_t const& node)
    {
        ASSERT(node.children.size() == 3,"");

        return "a primary flavor that " + print_simple_flavor(node.children[0].children[0])
            + ", undertones that " + print_simple_flavor(node.children[1].children[0])
            + " and an aftertaste that " + print_simple_flavor(node.children[2].children[0]);
    }

    string print_simple_flavor(generated_node_t const& node)
    {
        auto sorted_flavors = get_sorted_flavors(node);

        ASSERT(sorted_flavors.size() > 0, "");

        if(sorted_flavors[0].percentage >= 90)
        {
            return "tastes very " + sorted_flavors[0].flavor_string;
        }
        else if(sorted_flavors[0].percentage >= 70)
        {
            std::string ret = "tastes mostly " + sorted_flavors[0].flavor_string;

            if(sorted_flavors.size() >= 2 && sorted_flavors[1].percentage > 20)
            {
                ret += " and slightly " + sorted_flavors[1].flavor_string;
            }

            return ret;
        }
        //else if(sorted_flavors[0].percentage >= 50)
        else
        {
            std::string ret = "tastes somewhat " + sorted_flavors[0].flavor_string;

            if(sorted_flavors.size() >= 2 && sorted_flavors[1].percentage > 20)
            {
                ret += " and slightly " + sorted_flavors[1].flavor_string;
            }

            return ret;
        }
    }

    string print_flavor(generated_node_t const& node)
    {
        if(node.children.size() > 1)
        {
            return print_complex_flavor(node);
        }
        else
        {
            return print_simple_flavor(node.children[0]);
        }
    }
    
    string print_taste(generated_node_t const& node)
    {
        if(node.children[0].name == "Dangerous Taste")
        {
            auto const& edibility = node.children[0].children[0];
            auto const& flavor = node.children[0].children[1];
            return "is " + edibility.generated_values[0]
                + " but tastes " + flavor.generated_values[0];
        }
        else
        {
            return print_flavor(node.children[0]);
        }
    }

    string print_color(generated_node_t const& node)
    {
        return "TODO: color";
    }

    string print_basic_property(generated_node_t const& node)
    {
        ASSERT(node.children.empty(), "");
        ASSERT(node.generated_values.size() > 0, "");


        if(node.generated_values.size() == 1)
        {
            return node.name + " that is " + node.generated_values[0];
        }
        else
        {
            string property_string =  node.name + " that is ";

            for(size_t i = 0; i < node.children.size() - 1; ++i)
            {
                property_string += node.generated_values[i] + ", ";
            }

            property_string += "and " + node.generated_values.back();
            return property_string;
        }

        return "";
    }

    string print_sub_property(generated_node_t const& node)
    {
        if(node.children.empty())
            return print_basic_property(node);

        auto const& child = node.children[0];
        std::string summary;

        summary = "The " + node.name + " is a " + child.name + " that:";

        for(auto const& child : node.children)
        {
            if(child.name == "Taste")
                summary += print_taste(child);
            else if(child.name == "Color")
                summary += print_color(child);
            else
                summary += print_sub_property(child);
        }

        return summary;
    }

    string print_plant(generated_node_t const& plant_node)
    {
        string summary = plant_node.name + ":\n";

        string root_summary;
        string body_summary;
        string fruit_summary;

        //not the most efficient
        for(auto const& child : plant_node.children)
        {
            if(child.name == "Root")
                root_summary = print_sub_property(child);
            else if(child.name == "Body")
                body_summary = print_sub_property(child);
            else if(child.name == "Fruit")
                fruit_summary = print_sub_property(child);
        }

        return summary + "\n"
             + root_summary + "\n"
             + body_summary + "\n"
             + fruit_summary;
    }
}