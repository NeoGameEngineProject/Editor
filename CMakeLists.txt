project(NeoEditor)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/CMake
	${CMAKE_MODULE_PATH}
)

include(CopyFiles)

set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS TRUE)
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
set(DISABLE_MULTITHREAD TRUE)

add_subdirectory(SDK)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

copy_files("${CMAKE_CURRENT_SOURCE_DIR}/assets/*" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" copy-editor-data)

file(GLOB_RECURSE SOURCES src/*.c*)
file(GLOB_RECURSE UI src/*.ui)

file(GLOB IMGUI_SOURCES imgui-external/*.c*)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5OpenGL CONFIG REQUIRED)
find_package(Qt5Sql CONFIG REQUIRED)

add_definitions(-D__gl3w_h_ -DQT_NO_FOREACH)
add_executable(NeoEditor WIN32 ${SOURCES} ${UI} ${IMGUI_SOURCES} BreezeStyleSheets/breeze.qrc)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	target_link_libraries(NeoEditor Qt5::Widgets Qt5::OpenGL Qt5::Sql "-Wl,-rpath ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" -Wl,--whole-archive NeoEngine NeoScript -Wl,--no-whole-archive)
else()
	target_link_libraries(NeoEditor Qt5::Widgets Qt5::OpenGL Qt5::Sql NeoEngine NeoScript)
endif()

target_include_directories(NeoEditor PUBLIC src src/ImGuizmo imgui-external)
add_dependencies(NeoEditor build-shaders copy-editor-data)

install(TARGETS NeoEditor DESTINATION bin)
install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets DESTINATION bin)
