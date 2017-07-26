#include "plant_printer.h"

#include <algorithm>
#include <iostream>

#include "asdf_multiplat/main/asdf_defs.h"
#include "asdf_multiplat/utilities/utilities.h"

using namespace std;
using namespace plantgen;
using namespace asdf::util;

namespace plant_printer
{
    namespace
    {
        constexpr char string_var_delimiter = '%';
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
                // return true if the left should be ordered before the second
                return lhs.percentage > rhs.percentage;
            });

        return sorted_values;
    }



    string print_range_value(generated_node_t const& range)
    {
        auto sorted_values = get_sorted_range(range);

        ASSERT(sorted_values.size() > 0, "");

        if(sorted_values.size() == 1)
        {
            return sorted_values[0].range_string;
        }


        size_t div = 100 / sorted_values.size();
        int largest_diff = 0;
        for(auto const& sorted : sorted_values)
        {
            int diff = sorted.percentage - sorted_values[0].percentage;
            largest_diff = std::max(abs(diff), largest_diff);
        }


        if(largest_diff <= 4)
        {
            vector<string> strs;
            strs.reserve(sorted_values.size());
            for(auto const& sorted : sorted_values)
            {
                strs.push_back(sorted.range_string);
            }

            return "equal parts " + combine_strings_with_comma_and(strs);
        }
        else if(sorted_values[0].percentage >= 95)
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
        else if(sorted_values[0].percentage >= 45
            && (sorted_values.size() > 0 && sorted_values[1].percentage > 30))
        {
            return sorted_values[0].range_string + " and " + sorted_values[1].range_string;
        }
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

        property_string += combine_strings_with_comma_and(node.generated_values);

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

    
    string print_sub_property(generated_node_t const& node, size_t level, print_flags_t print_flags)
    {
        std::string summary = indenation_string(level);

        if(node.print_string.size() > 0)
        {
            summary += print_property_with_string(node);
        }
        else
        {
            //if not flagged to skip the name, print it
            if((print_flags & print_flags_skip_name) == 0)
                summary += node.name_string() + ": ";

            if(node.children.empty() && node.value_nodes.empty())
            {
                summary += print_basic_property(node);
            }
            else
            {
                //change trailing space to a newline if we are printing a sub-property of node
                if(summary.size() > 0 && summary.back() == ' ')
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


    enum print_string_modifiers_e
    {
          no_modifier = 0
        , use_node_first_value = 1
        // = 2
        // = 4
    };

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
            uint32_t modifiers = 0;
            if(var_loc.second.size() > 0)
            {
                modifiers |= (use_node_first_value * (var_loc.second[0] == '$'));
            }


            std::string var_str;
            auto prev_size = final_string.size();

            if(var_loc.second == "Name")
            {
                final_string.replace(var_loc.first + adjust, 4+2, node.name);
            }
            else
            {
                /// TODO: reorganize this
                ///       maybe move the handling of the '$' into
                ///       parse_print_string() instead of dealing with it here
                for(generated_node_t const& child : node.children)
                {
                    bool use_node_val = (modifiers & use_node_first_value) > 0;
                    string cmp;

                    if(use_node_val)
                    {
                        ASSERT(var_loc.second[0] == '$', "Expected $ at start of print string variable name");
                        cmp = var_loc.second.substr(1);
                    }
                    else
                    {
                        cmp = var_loc.second;
                    }


                    if(cmp == child.name)
                    {
                        string str;

                        if(use_node_val)
                        {
                            if(child.generated_values.size() > 0)
                                str = print_basic_property(child);
                            else if(child.value_nodes.size() > 0)
                            {
                                auto const* leaf_vn = leaf_value_node(child);
                                str = print_basic_property(*leaf_vn);
                            }
                            else
                            {
                                EXPLODE("asdf");
                            }
                        }
                        else
                        {
                            str = print_sub_property(child, 0, print_flags_skip_name);
                        }

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
        return "A " + plant_node.name + " " + print_sub_property(plant_node);
    }
}