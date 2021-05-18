cmake_minimum_required(VERSION 3.17 FATAL_ERROR)
cmake_policy(VERSION 3.17)
cmake_policy(SET CMP0074 NEW)
message(STATUS "Platform Generator : ${CMAKE_GENERATOR_PLATFORM}")
project(TabGraph LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
#option(BUILD_SHARED_LIBS "Build shared libraries ?" OFF)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

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

include(cmake/FetchDependencies.cmake)

add_subdirectory(TabGraph)
add_library(TabGraph::TabGraph ALIAS TabGraph)
add_library(TabGraph::Plugins ALIAS TabGraphPlugins)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(tests)
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Doxygen)

if (DOXYGEN_FOUND)
	set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
	set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)
	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
	message("Doxygen build started")
	add_custom_target(Documentation
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
	message("Doxygen not found")
endif (DOXYGEN_FOUND)
