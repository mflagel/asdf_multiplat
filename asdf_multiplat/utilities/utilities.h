#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <csignal>
#include <algorithm>

//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/uniform_int_distribution.hpp>

// #include "rapidjson/document.h"

//#define FOR_EACH(type, objName, container) std::for_each(container.begin(), container.end(), [&](type objName)
#define FOR_EACH(type, objName, container) std::for_each(std::begin(container), std::end(container), [&](type objName)
#define MEMBER_FUNCTION_PREDICATE(class, func) std::bind1st(std::mem_fun(&class::func),this)
#define DELETE_VECTOR_POINTERS(vector) for(size_t i = 0; i < vector.size(); i++){delete vector[i];}

#define OBJ_AS(objName, asType) ((asType) objAs = (asType)objName); objAs


#ifdef _MSC_VER
    #include <direct.h>
    #define GetCurrentDir _getcwd
    #define SetCurrentDir _chdir
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
    #define SetCurrentDir chdir
#endif


namespace asdf {
    namespace util {
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

    // rapidjson::Document read_json_file(std::string const& filepath);
    // void write_json_file(rapidjson::Document&, std::string const& filepath);

    std::string get_current_working_directory();
    bool file_exists(std::string const& filepath);

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

    std::vector<std::string> tokenize(char* const str, char* const delimiters);

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