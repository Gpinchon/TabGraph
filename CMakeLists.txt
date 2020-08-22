cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

project(TabGraph)

#set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install" CACHE PATH "Cmake prefix" FORCE)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
include(ExternalProject)

FetchContent_Declare(
	CMAKEMODULES
	GIT_REPOSITORY https://github.com/lbaehren/CMakeModules.git
)

FetchContent_Declare(
	SDL2_CMAKE
	GIT_REPOSITORY https://github.com/tcbrindle/sdl2-cmake-scripts.git
)

FetchContent_Declare(
	SDL2
	URL			https://www.libsdl.org/release/SDL2-devel-2.0.12-VC.zip
	URL_HASH	MD5=c2262a2f2d4e8487b75b541bfee3c2c4
)

FetchContent_Declare(
	SDL2_IMAGE
	URL			https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.5-VC.zip
	URL_HASH	MD5=b6c8c85b64e01b7096364036438b3333
)

FetchContent_Declare(
	GLEW
	URL			https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip
	URL_HASH	MD5=32a72e6b43367db8dbea6010cd095355
)

FetchContent_Declare(
	GLM
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG        0.9.9.8
)

FetchContent_Declare(
	RAPIDJSON
	GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
	GIT_TAG        v1.1.0
)

FetchContent_Declare(
	ZLIB
	URL			https://www.zlib.net/zlib-1.2.11.tar.gz
	URL_HASH	SHA256=c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1
)

option(USE_GDAL "Use Gdal for terrains" OFF)
if (USE_GDAL)
	add_definitions(-DUSE_GDAL)
	FetchContent_Declare(
		GDAL
		URL			http://download.gisinternals.com/sdk/downloads/release-1911-x64-gdal-3-1-2-mapserver-7-6-1-libs.zip
		URL_HASH    MD5=EB26FA4603DFD9F84FC53D6678C7F7C1
	)
	FetchContent_GetProperties(GDAL)
	if(NOT GDAL_POPULATED)
		FetchContent_Populate(GDAL)
		set(GDAL_ROOT PATH "${gdal_SOURCE_DIR}")
	endif()
	find_package(GDAL REQUIRED)
	add_dependencies(TabGraphObjects GDAL::GDAL)
	target_link_libraries(TabGraphObjects ${GDAL_LIBRARIES})
endif()

FetchContent_GetProperties(CMAKEMODULES)
if(NOT CMAKEMODULES_POPULATED)
	FetchContent_Populate(CMAKEMODULES)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${cmakemodules_SOURCE_DIR}")
endif()

FetchContent_GetProperties(SDL2_CMAKE)
if(NOT SDL2_CMAKE_POPULATED)
	FetchContent_Populate(SDL2_CMAKE)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${sdl2_cmake_SOURCE_DIR}")
endif()

FetchContent_GetProperties(SDL2)
if(NOT SDL2_POPULATED)
	FetchContent_Populate(SDL2)
	set(SDL2_ROOT PATH "${sdl2_SOURCE_DIR}")
endif()

FetchContent_GetProperties(SDL2_IMAGE)
if(NOT SDL2_IMAGE_POPULATED)
	FetchContent_Populate(SDL2_IMAGE)
	set(SDL2_IMAGE_PATH PATH "${sdl2_image_SOURCE_DIR}")
endif()

FetchContent_GetProperties(GLEW)
if(NOT GLEW_POPULATED)
	FetchContent_Populate(GLEW)
	set(GLEW_ROOT PATH "${glew_SOURCE_DIR}")
endif()

FetchContent_GetProperties(GLM)
if(NOT GLM_POPULATED)
	FetchContent_Populate(GLM)
	set(glm_DIR "${glm_SOURCE_DIR}/cmake/glm")
	#set(GLM_ROOT PATH "${glm_SOURCE_DIR}")
endif()

FetchContent_GetProperties(RAPIDJSON)
if(NOT RAPIDJSON_POPULATED)
	FetchContent_Populate(RAPIDJSON)
	set(RAPIDJSON_ROOT PATH "${rapidjson_SOURCE_DIR}")
endif()

FetchContent_GetProperties(ZLIB)
if(NOT ZLIB_POPULATED)
	FetchContent_Populate(ZLIB)
	option(SKIP_INSTALL_ALL "Skip Zlib installs" ON)
	add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
	set(ZLIB_FOUND 1)
	set(ZLIB_LIBRARIES zlibstatic)
	set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}")
endif()

ExternalProject_Add(
    RapidJSON
    SOURCE_DIR ${rapidjson_SOURCE_DIR}
    BINARY_DIR ${rapidjson_SOURCE_DIR}
    TIMEOUT 10
    CMAKE_ARGS
        -DRAPIDJSON_BUILD_TESTS=OFF
        -DRAPIDJSON_BUILD_DOC=OFF
        -DRAPIDJSON_BUILD_EXAMPLES=OFF
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

# Prepare RapidJSON (RapidJSON is a header-only library)
#ExternalProject_Get_Property(rapidjson source_dir)
set(RAPIDJSON_INCLUDE_DIR ${rapidjson_SOURCE_DIR}/include)

#set(ZLIB_ROOT ${zlib_BINARY_DIR})
#find_package(ZLIB REQUIRED)

set(SDL2_BUILDING_LIBRARY ON)
find_package(SDL2 REQUIRED)

set(SDL2_IMAGE_BUILDING_LIBRARY ON)
find_package(SDL2_image REQUIRED)

find_package(GLEW REQUIRED)

find_package(glm REQUIRED)

#find_package(RAPIDJSON REQUIRED)

#RapidJson
#option(RAPIDJSON_BUILD_DOC "Build rapidjson documentation." OFF)
#option(RAPIDJSON_BUILD_EXAMPLES "Build rapidjson examples." OFF)
#option(RAPIDJSON_BUILD_TESTS "Build rapidjson perftests and unittests." OFF)
#option(RAPIDJSON_BUILD_THIRDPARTY_GTEST "Use gtest installation in `thirdparty/gtest` by default if available" OFF)
#add_subdirectory(libs/rapidjson)

#GLEW
#option(glew-cmake_BUILD_SHARED "Build the shared glew library" OFF)
#option(ONLY_LIBS "Do not build executables" ON)
#add_subdirectory(libs/glew)

#glm


#zlib

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

#find_package(GLM REQUIRED)
#message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

file(GLOB_RECURSE SHADER_FILES 
	${CMAKE_SOURCE_DIR}/shaders/*.glsl
	${CMAKE_SOURCE_DIR}/shaders/*.frag
	${CMAKE_SOURCE_DIR}/shaders/*.vert
	)

file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.cpp
	)

file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/include/*.h
	${CMAKE_SOURCE_DIR}/include/*.hpp
	)



add_definitions(-DNOMINMAX)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  	add_definitions(-DDEBUG_MOD)
endif()
if(MSVC_IDE)
	set(CMAKE_DEBUG_POSTFIX "d")
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "shaders" "Shaders" groupname ${groupname})
				string(REPLACE "src" "Sources" groupname ${groupname})
				string(REPLACE "include" "Includes" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources(src)
	GroupSources(shaders)
	GroupSources(include)
endif()

add_library(TabGraphObjects OBJECT ${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

target_include_directories(TabGraphObjects PUBLIC
	include
	shaders
	${RAPIDJSON_INCLUDE_DIR}
	#${GLEW_INCLUDE_DIRS}
	#${GLM_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIR}
	)


#add_dependencies(TabGraphObjects ${ZLIB_LIBRARIES})
#add_dependencies(TabGraphObjects ${SDL2_LIBRARY})
#add_dependencies(TabGraphObjects ${SDL2_IMAGE_LIBRARIES})
add_dependencies(TabGraphObjects RapidJSON)
add_dependencies(TabGraphObjects glm::glm)
add_dependencies(TabGraphObjects GLEW::GLEW)
add_dependencies(TabGraphObjects zlibstatic)
add_dependencies(TabGraphObjects glm::glm)
add_dependencies(TabGraphObjects GLEW::GLEW)

target_link_libraries(TabGraphObjects glm::glm)
target_link_libraries(TabGraphObjects GLEW::GLEW)
target_link_libraries(TabGraphObjects zlibstatic)
target_link_libraries(TabGraphObjects ${SDL2_LIBRARY})
target_link_libraries(TabGraphObjects ${SDL2_IMAGE_LIBRARIES})

#target_link_libraries(TabGraphObjects ${RAPIDJSON_LIBRARIES})
#target_link_libraries(TabGraphObjects ${GLEW_LIBRARIES})
#target_link_libraries(TabGraphObjects ${ZLIB_LIBRARIES})
