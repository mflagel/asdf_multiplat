cmake_minimum_required(VERSION 2.8)

project(gurps_track)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB HEADER_FILES 	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
						"${CMAKE_CURRENT_SOURCE_DIR}/data/*.h"
					    "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.h")

# according to cmake docs, using glob in this manner is bad
# but this is how kiui uses it
file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
					   "${CMAKE_CURRENT_SOURCE_DIR}/data/*.cpp"
					   "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.cpp"
					   "${CMAKE_CURRENT_SOURCE_DIR}/../../ext/cJSON/cJSON.c"
					   "${CMAKE_CURRENT_SOURCE_DIR}/../../asdf_multiplat/utilities/str_const.cpp"
					   "${CMAKE_CURRENT_SOURCE_DIR}/../../asdf_multiplat/utilities/utilities.cpp"
					   )


if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=gnu++14)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++14)
endif ()

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-comment -fms-extensions -Wno-invalid-token-paste" )


set(CMAKE_BUILD_TYPE Debug)

add_executable(gurps_track ${SOURCE_FILES})

# new includes
include_directories("${CMAKE_SOURCE_DIR}/../../include")
include_directories("${CMAKE_SOURCE_DIR}/../../include/kiui")
include_directories("${CMAKE_SOURCE_DIR}/../../ext")
include_directories("${CMAKE_SOURCE_DIR}/../../")

target_link_libraries(gurps_track kiui)  # gurps_track is target, kiui is package to link

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

install(TARGETS gurps_track RUNTIME DESTINATION bin)