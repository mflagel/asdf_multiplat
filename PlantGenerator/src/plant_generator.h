#pragma once

#ifdef _MSC_VER
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace stdfs = std::experimental::filesystem;

namespace plantgen
{
    
    struct multi_value_t
    {
        size_t num_to_pick;

        std::vector<std::string> values;
    };

    using null_value_t = int;
    using range_value_t = std::vector<std::string>;
    using variant_value_t = std::variant<std::string, range_value_t, multi_value_t, null_value_t>;
    using value_list_t = std::vector<variant_value_t>;


    /// curiously recurring template pattern (sort-of)
    template <typename T>
    struct base_node_t
    {
        T* parent = nullptr;
        std::vector<T> children;
        std::string name;

        void add_child(T&& c)
        {
            children.push_back(std::move(c));
            children.back().parent = static_cast<T*>(this);
        }
    };


    // store value nodeas separately, since I can't store nodes in
    // the variant without them being heap-allocated
    // (since AFAIK if the variant stores a node, which stores the
    // variant, it can lead to an infinite size)
    struct pregen_node_t : base_node_t<pregen_node_t>
    {
        value_list_t values;
        std::vector<pregen_node_t> value_nodes;
    };

    struct generated_node_t : base_node_t<generated_node_t>
    {
        std::vector<std::string> generated_values;
    };


    generated_node_t generate_node(pregen_node_t const& node);
    generated_node_t generate_node_from_file(stdfs::path const& filepath);

    generated_node_t roll_values(pregen_node_t const& node);

    void print_node(generated_node_t const& node, size_t level = 0);


    struct file_not_found_exception : public std::runtime_error
    {
        stdfs::path path;

        file_not_found_exception(stdfs::path const& _path)
        : std::runtime_error("File Not Found: \"" + _path.string() + "\"")
        , path(_path)
        {}
    };

    struct include_exception : public std::runtime_error
    {
        stdfs::path include_from_path;
        stdfs::path include_path;

        include_exception(stdfs::path const& _inc_from, stdfs::path const& _inc)
        : std::runtime_error("File Not Found: " + _inc.string() + "\n"
                           + "Included by " + _inc_from.string() + "\n")
        , include_from_path(_inc_from)
        , include_path(_inc)
        {}
    };

    struct json_parse_exception : public std::runtime_error
    {
        stdfs::path json_filepath;
        const char* cjson_error_cstr;

        json_parse_exception(stdfs::path const& _json_filepath, const char* _cjson_err = nullptr)
        : std::runtime_error("JSON Parse Error: JSON file is invalid: " + _json_filepath.string()
                        + "\n--- cJSON Error ---\n" + _cjson_err)
        , json_filepath(_json_filepath)
        , cjson_error_cstr(_cjson_err)
        {}
    };
}