project(NeoEditor C CXX)
cmake_minimum_required(VERSION 3.12.4)

set(CMAKE_MODULE_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/CMake
	${CMAKE_MODULE_PATH}
)

if(APPLE AND EXISTS /usr/local/opt/qt)
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt")
endif()

include(CopyFiles)

set(CMAKE_CXX_STANDARD 17)
set(NEO_BUILD_SHARED_LIBS TRUE)
set(BUILD_SHARED_LIBS TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_MACOSX_RPATH TRUE)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,.,-rpath,../lib")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,.,-rpath,../lib")

if(ENABLE_SANITIZERS)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${SANITIZER} -fsanitize=undefined")
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
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

copy_files("${CMAKE_CURRENT_SOURCE_DIR}/assets/*" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" copy-editor-data)

file(GLOB_RECURSE SOURCES src/*.c*)
file(GLOB_RECURSE UI src/*.ui)

file(GLOB IMGUI_SOURCES imgui-external/*.c*)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets OpenGL Sql REQUIRED)
#find_package(Qt5Widgets CONFIG REQUIRED)
#find_package(Qt5OpenGL CONFIG REQUIRED)
#find_package(Qt5Sql CONFIG REQUIRED)

add_definitions(-D__gl3w_h_ -DQT_NO_FOREACH)
add_executable(NeoEditor WIN32 ${SOURCES} ${UI} ${IMGUI_SOURCES} BreezeStyleSheets/breeze.qrc)

if(ENABLE_SANITIZERS)
	target_compile_options(NeoEditor PUBLIC -fsanitize=${SANITIZER} -fsanitize=undefined -fno-omit-frame-pointer)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT APPLE)
	target_link_libraries(NeoEditor Qt5::Widgets Qt5::OpenGL Qt5::Sql "-Wl,-rpath ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" -Wl,--whole-archive NeoEngine NeoScript -Wl,--no-whole-archive)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT APPLE)
	target_link_libraries(NeoEditor "-pagezero_size 10000 -image_base 100000000" Qt5::Widgets Qt5::OpenGL Qt5::Sql "-Wl,-rpath ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" -Wl,--all_load NeoEngine NeoScript -Wl,-noall_load)
else()
	target_link_libraries(NeoEditor Qt5::Widgets Qt5::OpenGL Qt5::Sql NeoEngine NeoScript)
endif()

target_include_directories(NeoEditor PUBLIC src src/ImGuizmo imgui-external)
add_dependencies(NeoEditor build-shaders copy-editor-data)

install(TARGETS NeoEditor DESTINATION bin)
install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets DESTINATION bin)
