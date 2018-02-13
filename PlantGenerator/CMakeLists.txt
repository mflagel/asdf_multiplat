### PlantGenerator CMake Config ###

cmake_minimum_required (VERSION 3.0.2)

# TODO check for minimum complier version
# or more specifically, check for c++17

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)


project(plantgen)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

include_directories(..)
include_directories(../asdf_multiplat/src)
include_directories(SYSTEM ../include)


include_directories(src)
include_directories(../ext)
file(GLOB sources "src/*.cpp")

set(tidy_sources ${sources})
set(sources ${sources} ${CMAKE_SOURCE_DIR}/../ext/cJSON/cJSON.c)





add_executable(PlantGenerator ${sources})
# add_library(libplantgen STATIC ${sources})
# add_library(libplantgen SHARED ${sources})

# set c++ standard to C++17
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif(MSVC)


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
    target_link_libraries(PlantGenerator ${CMAKE_SOURCE_DIR}/../lib/win32/x64/AsdfMultiplat_static_D.lib)
    # target_link_libraries(PlantGenerator C:/Users/Mathias/Documents/Programming/AsdfMultiplat/lib/win32/x64/AsdfMultiplat_static_D.lib)
else()
    target_link_libraries(PlantGenerator stdc++fs)
endif()


# clang-tidy stuff
# https://stackoverflow.com/a/36798443
# http://www.mariobadr.com/using-clang-tidy-with-cmake-36.html

# clang-tidy <source-files> -dump-config <tidy-options> -- <compile-options> > .clang-tidy
# clang-tidy ${tidy_sources} -dump-config -- ${CMAKE_CXX_FLAGS} > .clang-tidy


find_program(
  CLANG_TIDY_EXE
  NAMES "clang-tidy"
  DOC "Path to clang-tidy executable"
  )
if(NOT CLANG_TIDY_EXE)
  message(STATUS "clang-tidy not found.")
else()
  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" )
endif()

set_target_properties(
  PlantGenerator PROPERTIES
  CXX_STANDARD_REQUIRED ON
  COMPILE_FLAGS "${WARNING_FLAGS} ${CMAKE_CXX_FLAGS}"
)

if(CLANG_TIDY_EXE)
  set_target_properties(
    PlantGenerator PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
endif()


add_custom_target(
  clang-tidy-config
  COMMAND ${CLANG_TIDY_EXE}
          ${tidy_sources}
          -dump-config
          --
          -std=c++1z
          -I${INCLUDE_DIRECTORIES}
          -I/usr/include/clang/3.8.0/include
          -I/usr/local/include/c++/8.0.0
          > .clang-tidy
)

add_custom_target(
  clang-tidy
  COMMAND ${CLANG_TIDY_EXE}
          ${tidy_sources}
          -config=''
          --
          -std=c++1z
          -I${INCLUDE_DIRECTORIES}
          -I/usr/include/clang/3.8.0/include
          -I/usr/local/include/c++/8.0.0
)
