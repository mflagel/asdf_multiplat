### PlantGenerator CMake Config ###

cmake_minimum_required (VERSION 3.0.2)


project(plantgen)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

include_directories(..)
include_directories(../asdf_multiplat)
include_directories(../include)


include_directories(src)
include_directories(../ext)
file(GLOB sources "src/*.cpp")

set(sources ${sources} ${CMAKE_SOURCE_DIR}/../ext/cJSON/cJSON.c)


add_executable(PlantGenerator ${sources})
# add_library(libplantgen STATIC ${sources})
# add_library(libplantgen SHARED ${sources})

# set_property(TARGET PlantGenerator PROPERTY CXX_STANDARD 17)
# set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

if(WIN32)
    # target_link_libraries(PlantGenerator ${CMAKE_SOURCE_DIR}/../lib/win32/x64/AsdfMultiplat_static_D.lib)
    target_link_libraries(PlantGenerator C:/Users/Mathias/Documents/Programming/AsdfMultiplat/lib/win32/x64/AsdfMultiplat_static_D.lib)
endif(WIN32)