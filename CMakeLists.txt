project(NeoEditor)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_SHARED_LIBS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

if(ENABLE_SANITIZERS)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

set(NO_DOCS TRUE)
set(NO_TESTGAME TRUE)
set(NO_TESTS TRUE)
set(DISABLE_RENDER_THREAD TRUE)
set(OpenGL_GL_PREFERENCE LEGACY)
set(ENABLE_OPENGL_RENDERER TRUE)
set(ENABLE_DUMMY_PLATFORM TRUE)

add_subdirectory(SDK EXCLUDE_FROM_ALL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SOURCES src/*.c*)
file(GLOB_RECURSE UI src/*.ui)

file(GLOB IMGUI_SOURCES imgui-external/*.c*)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5OpenGL CONFIG REQUIRED)

add_definitions(-D__gl3w_h_)
add_executable(NeoEditor WIN32 ${SOURCES} ${UI} ${IMGUI_SOURCES} BreezeStyleSheets/breeze.qrc)
target_link_libraries(NeoEditor Qt5::Widgets Qt5::OpenGL NeoEngine)
target_include_directories(NeoEditor PUBLIC src src/ImGuizmo imgui-external)

add_dependencies(NeoEditor build-shaders)
