#include "plant_printer.h"

#include <algorithm>
#include <iostream>

#include "asdf_multiplat/main/asdf_defs.h"

using namespace std;
using namespace plantgen;

namespace plant_printer
{
    namespace
    {
        constexpr char string_var_delimiter = '%';
    }

    template<typename L>
    string combine_strings_with(L const& strings, string const& spacer, string const& final_spacer)
    {
        string s;

        for(size_t i = 0; i < strings.size() - 1; ++i)
        {
            s += strings[i] + spacer;
        }

        s.resize(s.size() - 2);
        s += final_spacer + strings.back();

        return s;
    }

    string combine_strings_with(vector<string> const& strings, string const& spacer)
    {
        return combine_strings_with(strings, spacer, spacer);
    }

    //the most common usage of combine_strings_with gets its own func
    string combine_strings_with_comma_and(vector<string> const& strings)
    {
        return combine_strings_with(strings, ", ", " and ");
    }


    string_var_locations_t parse_print_string(string const& print_string)
    {
        string_var_locations_t var_locations;

        size_t pos = 0;
        size_t endpos = 0;

        while(true)
        {
            pos = print_string.find(string_var_delimiter, endpos);
            if(pos == string::npos)
                break;

            endpos = print_string.find(string_var_delimiter, pos+1);

            if(endpos == string::npos)
            {
                EXPLODE("unmached %%");
            }

            ASSERT(endpos > pos, "");
            ASSERT(endpos-pos >= 1, "");
            std::string key = print_string.substr(pos+1, endpos-pos-1);
            var_locations.insert({key,pos});

            endpos++; //move it past the percent sign for the next find call
        }

        return var_locations;
    }


    struct sorted_range_value_t
    {
        int percentage;
        std::string range_string;
    };

    std::vector<sorted_range_value_t> get_sorted_range(generated_node_t const& node)
    {
        if(node.generated_values.size() == 0)
            return std::vector<sorted_range_value_t>();

        //ASSERT(node.generated_values.size() > 0, "expected some flavors to sort");
        std::vector<sorted_range_value_t> sorted_values;
        sorted_values.reserve(node.generated_values.size());
        
        for(auto const& val : node.generated_values)
        {
            auto percent_sign_pos = val.find('%');

            ASSERT(percent_sign_pos != std::string::npos, "");
            if(percent_sign_pos != std::string::npos)
            {
                sorted_range_value_t sv =
                {
                    stoi(val.substr(0, percent_sign_pos)),
                    val.substr(percent_sign_pos + 2)
                };

                sorted_values.push_back(sv);
            }
        }

        std::sort(sorted_values.begin(), sorted_values.end(),
            [](sorted_range_value_t const& lhs, sorted_range_value_t const& rhs)
            {
                return lhs.percentage > rhs.percentage;
            });

        return sorted_values;
    }


    string print_range_value(generated_node_t const& range)
    {
        auto sorted_values = get_sorted_range(range);

        ASSERT(sorted_values.size() > 0, "");

        if(sorted_values[0].percentage >= 95)
        {
            return "completely " + sorted_values[0].range_string;
        }
        else if(sorted_values[0].percentage >= 90)
        {
            return "very " + sorted_values[0].range_string;
        }
        else if(sorted_values[0].percentage >= 70)
        {
            std::string ret = "mostly " + sorted_values[0].range_string;

            if(sorted_values.size() >= 2 && sorted_values[1].percentage > 20)
            {
                ret += " and slightly " + sorted_values[1].range_string;
            }

            return ret;
        }
        //else if(sorted_values[0].percentage >= 50)
        else
        {
            std::string ret = "somewhat " + sorted_values[0].range_string;

            if(sorted_values.size() >= 2 && sorted_values[1].percentage > 20)
            {
                ret += " and slightly " + sorted_values[1].range_string;
            }

            return ret;
        }
    }

    string print_multi_value(generated_node_t const& node)
    {
        if(node.generated_values.empty())
            return "";

        string property_string;// =  node.name + " that is ";

        if(node.generated_values.size() == 1)
            return node.generated_values[0];

        for(size_t i = 0; i < node.generated_values.size() - 1; ++i)
        {
            property_string += node.generated_values[i] + ", ";
        }

        property_string.resize(property_string.size() - 2);
        property_string += " and " + node.generated_values.back();

        return property_string;
    }

    string print_basic_property(generated_node_t const& node)
    {
        ASSERT(node.children.empty(), "");

        if(node.generated_values.size() == 1)
        {
            return node.generated_values[0];
        }
        else if(node.generated_values.size() > 1)
        {
            // if there is a % in the first few characters
            // (ie: if its XXX% or XX% or X%)
            if(node.generated_values[0].find('%') <= 3)
            {
                return print_range_value(node);
            }
            else
            {
                return print_multi_value(node);
            }
        }

        return "";
    }

    // string to_string(generated_node_t const& node, size_t depth = std::numeric_limits<size_t>::max, size_t level = 0)

    
    string print_sub_property(generated_node_t const& node, size_t level)
    {
        std::string summary = indenation_string(level);

        if(node.print_string.size() > 0)
        {
            summary += print_property_with_string(node);
        }
        else
        {
            summary += node.name_string() + ": ";

            if(node.children.empty() && node.value_nodes.empty())
            {
                summary += print_basic_property(node);
            }
            else
            {
                //change trailing space to a newline if we are printing a sub-property of node
                ASSERT(summary.back() == ' ', "");
                summary.back() = '\n';

                for(auto const& child : node.children)
                    summary += print_sub_property(child, level + 1) + "\n";
                for(auto const& vn : node.value_nodes)
                    summary += print_sub_property(vn, level + 1) + "\n";

                summary.resize(summary.size() - 1);
            }
        }

        return summary;
    }


    /// TODO: move to util file
    /// https://stackoverflow.com/a/5056797
    template<typename A, typename B>
    std::pair<B,A> flip_pair(const std::pair<A,B> &p)
    {
        return std::pair<B,A>(p.second, p.first);
    }

    template<typename A, typename B>
    std::multimap<B,A> flip_map(const std::map<A,B> &src)
    {
        std::multimap<B,A> dst;
        std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), 
                       flip_pair<A,B>);
        return dst;
    }
    ///



    string print_property_with_string(generated_node_t const& node)
    {
        WARN_IF(node.print_string.empty(), "node has no print string in a function that is based around having one");
        return print_property_with_string(node, node.print_string);
    }

    string print_property_with_string(generated_node_t const& node, string const& print_string)
    {
        if(print_string.empty())
            return "";

        std::string final_string = print_string;

        // sort locations by string position (earliest to latest)
        // so that I can adjust the string replacement positions
        // as the string changes size
        auto var_locations = parse_print_string(print_string);
        auto sorted_locs = flip_map(var_locations);
        int adjust = 0;

        for(auto const& var_loc : sorted_locs)
        {
            std::string var_str;
            auto prev_size = final_string.size();

            if(var_loc.second == "Name")
            {
                final_string.replace(var_loc.first + adjust, 4+2, node.name);
            }
            else
            {
                for(generated_node_t const& child : node.children)
                {
                    if(var_loc.second == child.name)
                    {
                        //string str = str_for_var(child);
                        string str = print_sub_property(child);
                        final_string.replace(var_loc.first + adjust, var_loc.second.size()+2, str);
                        break;
                    }
                }
            }

            adjust += int(final_string.size()) - int(prev_size);
        }

        return final_string;
    }


    string print_plant(generated_node_t const& plant_node)
    {
        return "This " + plant_node.name + " " + print_sub_property(plant_node);
    }
}