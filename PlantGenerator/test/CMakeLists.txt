### PlantGenerator Test CMake Config ###

cmake_minimum_required (VERSION 3.0.2)

set(CMAKE_C_COMPILER /usr/local/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(plantgen_test)
set(CMAKE_BUILD_TYPE Debug)

include_directories(../..)
include_directories(../../asdf_multiplat)
include_directories(../../include)
include_directories(../../ext)

include_directories(src)
include_directories(../src)

file(GLOB plantgen_sources "../src/plant_generator.cpp"
                           "../src/from_json.cpp")

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