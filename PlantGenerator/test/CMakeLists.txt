### PlantGenerator Test CMake Config ###

cmake_minimum_required (VERSION 3.0.2)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(plantgen_test)
set(CMAKE_BUILD_TYPE Debug)

include_directories(../..)
include_directories(../../asdf_multiplat/src)
include_directories(../../include)
include_directories(../../ext)

include_directories(src)
include_directories(../src)

file(GLOB plantgen_sources "../src/plant_generator.cpp"
                           "../src/from_json.cpp"
                           "../src/value_types.cpp")

file(GLOB test_sources "src/*.cpp")
set(ext_sources ${CMAKE_SOURCE_DIR}/../../ext/cJSON/cJSON.c)

set(sources ${plantgen_sources} ${test_sources} ${ext_sources})


add_executable(PlantGenerator__Tests ${sources})


if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif(MSVC)



add_test(NAME "PlantGenerator__Tests_all"
    COMMAND plantgen_test
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)


# GCC and Clang compiler flags
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "CLANG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -Wextra \
        -Werror=return-type \
        -Werror=empty-body \
        -Wno-expansion-to-defined \
        -Wno-sign-compare")
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "CLANG")




# Libraries
if(WIN32)
    target_link_libraries(PlantGenerator__Tests ${CMAKE_SOURCE_DIR}/../lib/win32/x64/AsdfMultiplat_static_D.lib)
else()
    target_link_libraries(PlantGenerator__Tests stdc++fs)
endif()
