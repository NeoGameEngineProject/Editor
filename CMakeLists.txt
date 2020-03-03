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

# add_compile_definitions(VERBOSE_BUILD)

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,.,-rpath,../lib")
	
	# So tests can run
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,${CMAKE_BINARY_DIR}/bin,-rpath,../lib")
	# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=${SANITIZER}")

	if(ENABLE_SANITIZERS)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${SANITIZER} -fsanitize=undefined -fno-omit-frame-pointer")
	endif()
endif()

set(NO_DOCS FALSE)
set(NO_TESTGAME TRUE)
set(NO_TESTS FALSE)
set(DISABLE_RENDER_THREAD TRUE)
set(OpenGL_GL_PREFERENCE LEGACY)
set(ENABLE_OPENGL_RENDERER TRUE)
set(ENABLE_DUMMY_PLATFORM TRUE)
set(DISABLE_MULTITHREAD TRUE)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# Or else GCC LTO over multiple SO's will destroy the behavior registry
	# Clang with thinlto works fine though, so leave it enabled for that.
	set(NO_LTO TRUE)
endif()

add_subdirectory(SDK)

## Lua bindings for Qt5
set(LUA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/SDK/NeoScript/luajit-external/src" CACHE "" STRING FORCE)
set(QT_SCHEMA_DIR "${CMAKE_SOURCE_DIR}/lqt-external/generator/schema" CACHE "" STRING FORCE)

add_subdirectory(lqt-external EXCLUDE_FROM_ALL)
add_dependencies(qtcore luajit-build)

file(GLOB LQT_SRC lqt-external/common/*.cpp)
file(GLOB LQT_HPP lqt-external/common/*.hpp)
file(GLOB LQT_HPP lqt-external/common/*.inl)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

copy_files("${CMAKE_CURRENT_SOURCE_DIR}/assets/*" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" copy-editor-data)
copy_files("${CMAKE_CURRENT_SOURCE_DIR}/plugins/lua/*" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" copy-editor-plugins)

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
add_executable(NeoEditor WIN32 ${SOURCES} ${UI} src/neo.qrc ${IMGUI_SOURCES} ${LQT_SRC} ${LQT_HPP} BreezeStyleSheets/breeze.qrc)

## Add lqt as a dependency so plugins can be loaded
# add_dependencies(NeoEditor qtcore qtwidgets qtgui copy-editor-plugins copy-editor-data)
add_dependencies(NeoEditor copy-editor-plugins copy-editor-data)
add_custom_target(lqt-targets DEPENDS qtcore qtwidgets qtgui)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT APPLE)
	target_link_libraries(NeoEditor Qt5::Widgets Qt5::OpenGL Qt5::Sql "-Wl,-rpath ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" -Wl,--whole-archive NeoEngine NeoScript -Wl,--no-whole-archive)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT APPLE)
	target_link_libraries(NeoEditor "-pagezero_size 10000 -image_base 100000000" Qt5::Widgets Qt5::OpenGL Qt5::Sql "-Wl,-rpath ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" -Wl,--all_load NeoEngine NeoScript -Wl,-noall_load)
else()
	target_link_libraries(NeoEditor Qt5::Widgets Qt5::OpenGL Qt5::Sql NeoEngine NeoScript)
endif()

target_include_directories(NeoEditor PUBLIC src src/ImGuizmo imgui-external)
add_dependencies(NeoEditor build-shaders copy-editor-data)

if(WIN32)
	add_custom_command(TARGET NeoEditor POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:Qt5::Widgets>
			$<TARGET_FILE:Qt5::OpenGL>
			$<TARGET_FILE:Qt5::Sql>
			$<TARGET_FILE:Qt5::Core>
			$<TARGET_FILE:Qt5::Gui>

			$<TARGET_FILE_DIR:NeoEditor>
	)

	install(FILES
			$<TARGET_FILE:Qt5::Widgets>
			$<TARGET_FILE:Qt5::OpenGL>
			$<TARGET_FILE:Qt5::Sql>
			$<TARGET_FILE:Qt5::Core>
			$<TARGET_FILE:Qt5::Gui>
			
			DESTINATION bin)
endif()

install(TARGETS NeoEditor DESTINATION bin)

## Install lqt
install(TARGETS lfs jis gbk lrdb lpeglabel path miniz pinyin utf8 qtcore qtgui qtwidgets DESTINATION bin)

install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets DESTINATION bin)
install(DIRECTORY plugins DESTINATION bin)

if(INCLUDE_KITS)
	install(DIRECTORY "SDK/build-linux64-shared/linux64-shared" DESTINATION kits/linux64-shared)
	install(DIRECTORY "SDK/build-linux64-static/linux64-static" DESTINATION kits/linux64-static)
endif()
