#pragma once

#include <algorithm>
#include <csignal>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include <experimental/filesystem>

//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/uniform_int_distribution.hpp>

// #include "rapidjson/document.h"

#include "main/asdf_defs.h"

DIAGNOSTIC_PUSH
DIAGNOSTIC_IGNORE(-Wcomment)


#ifdef _MSC_VER
    #include <direct.h>
    #define GetCurrentDir _getcwd
    #define SetCurrentDir _chdir
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
    #define SetCurrentDir chdir
#endif



namespace std
{
template<>
struct hash<glm::ivec2> {
    size_t operator()(const glm::ivec2 &v) const {
        /// https://stackoverflow.com/a/17885727
        // return std::hash<int>()(v.x) ^ std::hash<int>()(v.y);

        // https://stackoverflow.com/a/2634715
        return (7919 + std::hash<int>()(v.x)) * 7919 + std::hash<int>()(v.y);
    }
};
}


namespace asdf {
    namespace util {
    /************************************************************************/
    /* String
    /************************************************************************/
    template<typename L>
    std::string combine_strings_with(L const& strings, std::string const& spacer, std::string const& final_spacer)
    {
        std::string s;

        for(size_t i = 0; i < strings.size() - 1; ++i)
        {
            s += strings[i] + spacer;
        }

        s.resize(s.size() - 2);
        s += final_spacer + strings.back();

        return s;
    }

    template<typename L>
    std::string combine_strings_with(L const& strings, std::string const& spacer)
    {
        return combine_strings_with(strings, spacer, spacer);
    }

    //the most common usage of combine_strings_with gets its own func
    template<typename L>
    std::string combine_strings_with_comma_and(L const& strings)
    {
        return combine_strings_with(strings, ", ", " and ");
    }

    inline bool str_eq(char const* a, char const* b)
    {
        return strcmp(a, b) == 0;
    }

    /************************************************************************/
    /* Misc
    /************************************************************************/
    template<class C, class T>
    inline auto contains_impl(const C& c, const T& x, int)
    -> decltype(c.find(x), true)
    { return end(c) != c.find(x); }

    template<class C, class T>
    inline bool contains_impl(const C& v, const T& x, long)
    { return end(v) != std::find(begin(v), end(v), x); }

    template<class C, class T>
    auto contains(const C& c, const T& x)
    -> decltype(end(c), true)
    { return contains_impl(c, x, 0); }



    // weird magical tuple iteration from 
    // http://codereview.stackexchange.com/a/67394
    template <typename Tuple, typename F, std::size_t ...Indices>
    void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>) {
        using swallow = int[];
        (void)swallow{1,
            (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...
        };
    }

    template <typename Tuple, typename F>
    void for_each(Tuple&& tuple, F&& f) {
        constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
        for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                      std::make_index_sequence<N>{});
    }
    //---


    /************************************************************************/
    /* Exception Handling
    /************************************************************************/
#ifndef _MSC_VER
    void terminate_handler() __attribute__((noreturn));
#else
    __declspec(noreturn) void terminate_handler();
#endif
    void interrupt_handler(int x);

    /************************************************************************/
    /* File IO
    /************************************************************************/
    std::string read_text_file(std::string const& filepath);
    void write_text_file(std::string const& filepath, std::string const& data);
    size_t read_binary_file(std::string const& filepath, char** data);
    void write_binary_file(std::string const& filepath, char* data, size_t n);

    // rapidjson::Document read_json_file(std::string const& filepath);
    // void write_json_file(rapidjson::Document&, std::string const& filepath);

    std::string get_current_working_directory();
    bool is_directory(std::string const& filepath);
    bool is_file(std::string const& filepath);

    std::string find_folder(std::string const& name, size_t max_search_dist = 5);
    void create_dir(std::string const& path);

    /// TODO: deprecate this func once MSVC and libstdc++ release their full <filesystem> implementations
    ///       (or at least their version of relative(path, path)
    std::experimental::filesystem::path
    relative(std::experimental::filesystem::path const& a, std::experimental::filesystem::path const& b);

    std::experimental::filesystem::path
    find_file(std::experimental::filesystem::path const& filename, std::experimental::filesystem::path const& start_point = std::experimental::filesystem::current_path());


    /************************************************************************/
    /* File Compression / Decompression (wraps zlib and libtar
    /************************************************************************/
    int compress_file(std::experimental::filesystem::path const& src_filepath
                    , std::experimental::filesystem::path const& dest_filepath
                    , int compression_level = -1) noexcept;
    int compress_file(FILE* src, FILE* dest, int compression_level);// noexcept;


    int decompress_file(std::experimental::filesystem::path const& src_filepath
                      , std::experimental::filesystem::path const& dest_filepath) noexcept;
    int decompress_file(FILE* src, FILE* dest) noexcept;

    int archive_files(std::vector<std::experimental::filesystem::path> const& filepaths
                    , std::experimental::filesystem::path const& combined_filepath);
    int unarchive_files(std::experimental::filesystem::path const& tar_path,
                        std::experimental::filesystem::path const& extract_dir);

    /************************************************************************/
    /* Random Number Generation
    /************************************************************************/
    //generate random velocity and angle between min and max
    //float RandomRange(boost::random::mt19937& random, float minValue, float maxValue){
    //	int temp = 1000000; //used to generate a random float between 0 and 1
    //	boost::random::uniform_int_distribution<> randomDistrib(0, temp);
    //	//min + rand / (maxRand/(max-min))
    //	return minValue + randomDistrib(random) / (float)(temp/(maxValue-minValue));	
    //}


    /************************************************************************/
    /* Bounds Checking
    /************************************************************************/
    template <class T>
    bool check_bounds(T v, T min, T max) {
        return v >= min && v <= max;
    }
    bool CheckBounds(int x, int y, int minX, int maxX, int minY, int maxY);
    bool CheckBounds(int x, int y, int z, int minX, int maxX, int minY, int maxY, int minZ, int maxZ);

    /************************************************************************/
    /* Iterates through all entries of a map and copies the keys into a
    /* std::vector of keys
    /************************************************************************/
    template <typename MapKeyType, typename MapValueType>
    std::vector<MapKeyType> GetMapKeys(std::map<MapKeyType, MapValueType> const& map)
    {
        std::vector<MapKeyType> out_keyList;

	    for_each(map.begin(), map.end(), [&](const std::pair<MapKeyType, MapValueType>& entry){
		    out_keyList.push_back(entry.first);
	    });

        return out_keyList;
    }


    std::vector<std::string> tokenize(const char* str, const char* delimiters);
    void replace(std::string& str, std::string const& to_replace, std::string const& replacement);

    /************************************************************************/
    /* SDL Utilities
    /************************************************************************/
    void checkSDLError(int line = -1);


    /************************************************************************/
    /* Box2D Utilities
    /************************************************************************/
#if ASDF_BOX2D == 1
    std::vector<b2Vec2> GetB2FixtureVerts(b2Fixture* fixture);
    size_t FindClosestVertexIndex(std::vector<b2Vec2> vertices, b2Vec2 positon);
#endif

    }

    /************************************************************************/
    /* Math Stuff     
    /************************************************************************/

    //copypasted signature from glm::gtx::spline.hpp
    //cubic bezier curve
    template <typename genType> 
    genType bezier(
        genType const & p0,
        genType const & p1,
        genType const & p2,
        genType const & p3,
        typename genType::value_type const & t)
    {
        typename genType::value_type a = (1.0f - t);

        return { (p0 *        a * a * a) 
               + (p1 * 3.0f * a * a * t)
               + (p2 * 3.0f * a * t * t)
               + (p3 *        t * t * t) };
    }

    //todo: template this
    uint64_t binomial_coefficient(uint64_t n, uint64_t k);

    // I could probably just abstract this out as a polynomial algorithm
    // todo: fix the issue that coefficient will get truncated when it gets downcasted to a float
    // todo; template this
    // glm::vec3 bezier(std::vector<glm::vec3> points, float t)
    // {
    //     glm::vec3 result{0.0f};

    //     float a = (1.0f - t);

    //     size_t degree = points.size();
    //     for(size_t i = 0; i < degree; ++i)
    //     {
    //         uint64_t coefficient = binomial_coefficient(degree, i);
    //         result += (points[i] * coefficient * glm::pow(a, float(degree - i)) * glm::pow(t, float(i)));
    //     }
    // }

}

DIAGNOSTIC_POP
