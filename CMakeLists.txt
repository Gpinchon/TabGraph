cmake_minimum_required(VERSION 3.17 FATAL_ERROR)
cmake_policy(VERSION 3.17)
cmake_policy(SET CMP0074 NEW)
message(STATUS "Platform Generator : ${CMAKE_GENERATOR_PLATFORM}")
project(TabGraph LANGUAGES CXX)

#option(BUILD_SHARED_LIBS "Build shared libraries ?" OFF)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Uncomment for debug
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)
set(CMAKE_DEBUG_TARGET_PROPERTIES INTERFACE_INCLUDE_DIRECTORIES)
# PREFER STATIC LIBRARIES ########
# cmake respects the order of extensions when looking for libraries
set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
# prefer linking against static libraries
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
# ------------------- ########
set(GLEW_USE_STATIC_LIBS ON)

add_definitions(-DGLEW_STATIC)
add_definitions(-DNOMINMAX)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DDEBUG_MOD")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_MOD")
set(CMAKE_CXX_FLAGS_RELEASE "-Ox")

include(FetchDependencies.cmake)

add_subdirectory(TabGraph)
add_library(TabGraph::TabGraph ALIAS TabGraph)
add_library(TabGraph::Plugins ALIAS TabGraphPlugins)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(tests)
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
#add_dependencies(Scene TabGraph)
#target_link_libraries(Scene Tabgraph)