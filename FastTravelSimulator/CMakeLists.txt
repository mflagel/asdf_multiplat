### FastTravelSimulator CMake Config ###

cmake_minimum_required (VERSION 3.0.2)

set(CMAKE_C_COMPILER /usr/local/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(fast_travel_sim)
set(CMAKE_BUILD_TYPE Debug)

include_directories(..)
include_directories(../asdf_multiplat)
include_directories(../asdf_multiplat/main)
include_directories(../include)

include_directories(src)
include_directories(../ext)
include_directories(../PlantGenerator/src)
include_directories(../../boost_1_64_0)

file(GLOB sources "src/*.cpp")

set(PlantgenSrc ../PlantGenerator/src)
set(plantgen_sources ${PlantgenSrc}/plant_generator.cpp
                     ${PlantgenSrc}/value_types.cpp
                     ${PlantgenSrc}/plant_printer.cpp
                     ${PlantgenSrc}/from_json.cpp)

set(sources ${sources} ${plantgen_sources} ${CMAKE_SOURCE_DIR}/../ext/cJSON/cJSON.c)


add_executable(fast_travel_sim ${sources})

# set c++ standard to C++17
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif(MSVC)


# GCC and Clang compiler flags
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "CLANG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "CLANG")


# Libraries
if(WIN32)
    target_link_libraries(fast_travel_sim ${CMAKE_SOURCE_DIR}/../lib/win32/x64/AsdfMultiplat_static_D.lib)
    # target_link_libraries(fast_travel_sim C:/Users/Mathias/Documents/Programming/AsdfMultiplat/lib/win32/x64/AsdfMultiplat_static_D.lib)
else()
    target_link_libraries(fast_travel_sim stdc++fs)
endif()