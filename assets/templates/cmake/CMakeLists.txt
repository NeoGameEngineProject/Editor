project(NeoGame)

cmake_minimum_required(VERSION 3.10)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${NEO_ROOT}/lib/cmake)

if(NEO_PLUGIN)
	message("-- Building plugin")
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(BUILD_SHARED_LIBS ON)
	
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath ${NEO_ROOT}/lib")
	endif()
else()
	message("-- Building executable")
	set(BUILD_SHARED_LIBS OFF)
endif()

find_package(NeoEngine REQUIRED)
find_package(NeoCore REQUIRED)
find_package(NeoScript REQUIRED)
find_package(NeoVR REQUIRED)
find_package(NeoHTML REQUIRED)

option(ENABLE_OPENGL_RENDERER "Use OpenGL" ON)
option(DISABLE_MULTITHREAD "Disable multithreading" OFF)

## Disable the test game
set(NO_TESTGAME ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "-flto -s ${CMAKE_CXX_FLAGS}")
	set(CMAKE_C_FLAGS "-flto -s ${CMAKE_C_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "-flto ${CMAKE_EXE_LINKER_FLAGS}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT EMSCRIPTEN)
	set(CMAKE_CXX_FLAGS "-flto=thin ${CMAKE_CXX_FLAGS}")
	set(CMAKE_C_FLAGS "-flto=thin ${CMAKE_C_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "-flto=thin -s ${CMAKE_EXE_LINKER_FLAGS}")
endif()

file(GLOB_RECURSE HEADERS game/*.h)
file(GLOB_RECURSE SOURCES game/*.c*)

if(NEO_PLUGIN)
	add_library(GamePlugin SHARED ${SOURCES} ${HEADERS})
	target_link_libraries(GamePlugin NeoEngine NeoScript NeoVR NeoHTML)
	target_compile_definitions(GamePlugin PRIVATE -DNEO_PLUGIN=1)
else()
	add_game(Game "${SOURCES};${HEADERS}" ${CMAKE_SOURCE_DIR}/../assets)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/game)
