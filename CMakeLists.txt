project(NeoEditor C CXX)
cmake_minimum_required(VERSION 3.12.4)

set(CMAKE_MODULE_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/CMake
	${CMAKE_MODULE_PATH}
)

SET(NEOEDITOR_MAJOR_VERSION 0)
SET(NEOEDITOR_MINOR_VERSION 6)
SET(NEOEDITOR_PATCH_VERSION 0)

set(NEOEDITOR_PACKAGE "Neo")
set(NEOEDITOR_VENDOR "Yannick Pflanzer")
set(NEOEDITOR_VERSION "${NEO_MAJOR_VERSION}.${NEO_MINOR_VERSION}.${NEO_PATCH_VERSION}")
set(NEOEDITOR_COPYRIGHT "Copyright (c) 2014-2018, ${NEO_VENDOR}.")

set(NEOEDITOR_CMAKE_EXPORT "lib/cmake")
set(CMAKE_INSTALL_LIBDIR "lib") # To prevent GNUInstallDirs from creating lib64

if(APPLE AND EXISTS /usr/local/opt/qt)
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt")
endif()

#if(WIN32)
#	find_program(LUA_CMD NAMES luajit.exe lua.exe lua5.1.exe lua51.exe)
#endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## Remove the "Debug" and "Release" directory prefixes on Windows
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

include(CopyFiles)

set(CMAKE_CXX_STANDARD 17)
set(NEO_BUILD_SHARED_LIBS TRUE)
set(BUILD_SHARED_LIBS TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_MACOSX_RPATH TRUE)

# add_compile_definitions(VERBOSE_BUILD)

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,${CMAKE_INSTALL_PREFIX}/lib,-rpath,${CMAKE_INSTALL_PREFIX}/bin,-rpath,.,-rpath,../lib")
	
	# So tests can run
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,${CMAKE_INSTALL_PREFIX}/lib,-rpath,${CMAKE_INSTALL_PREFIX}/bin,-rpath,${CMAKE_BINARY_DIR}/bin,-rpath,../lib")
	# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=${SANITIZER}")

	if(ENABLE_SANITIZERS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${SANITIZER} -fsanitize=undefined -fno-omit-frame-pointer")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=${SANITIZER} -fsanitize=undefined -fno-omit-frame-pointer")

		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${SANITIZER} -fsanitize=undefined -fno-omit-frame-pointer")
	endif()
endif()

set(NO_DOCS FALSE)
set(NO_TESTGAME TRUE)
set(NO_TESTS FALSE)
set(OpenGL_GL_PREFERENCE LEGACY)

set(ENABLE_OPENGL_RENDERER ON CACHE BOOL "" FORCE)
set(DISABLE_RENDER_THREAD OFF CACHE BOOL "" FORCE)

set(ENABLE_DUMMY_PLATFORM TRUE)
set(DISABLE_MULTITHREAD TRUE)
set(NO_SYSTEM_AL TRUE)

## For now, since clang does not support fat lto objects
## and dev plugins need to be compilable with debug flags
set(NO_LTO TRUE)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# Or else GCC LTO over multiple SO's will destroy the behavior registry
	# Clang with thinlto works fine though, so leave it enabled for that.
	set(NO_LTO TRUE)
endif()

add_subdirectory(SDK)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets OpenGL Sql Svg REQUIRED)
#find_package(Qt5Widgets CONFIG REQUIRED)
#find_package(Qt5OpenGL CONFIG REQUIRED)
#find_package(Qt5Sql CONFIG REQUIRED)

## Lua bindings for Qt5
set(LUA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/SDK/NeoScript/luajit-external/src" CACHE STRING "" FORCE)
set(LUA_LIBRARY luajit CACHE STRING "" FORCE)
set(LUA_CMD "${CMAKE_SOURCE_DIR}/SDK/NeoScript/luajit-external/src/luajit" CACHE STRING "" FORCE)
set(QT_SCHEMA_DIR "${CMAKE_SOURCE_DIR}/lqt-external/generator/schema" CACHE STRING "" FORCE)

add_subdirectory(lqt-external EXCLUDE_FROM_ALL)
add_dependencies(qtcore luajit-build)

## Because it uses STL features which were removed in 17
set_property(TARGET cpptoxml PROPERTY CXX_STANDARD 11)

file(GLOB LQT_SRC lqt-external/common/*.cpp)
file(GLOB LQT_HPP lqt-external/common/*.hpp)
file(GLOB LQT_HPP lqt-external/common/*.inl)

copy_files("${CMAKE_CURRENT_SOURCE_DIR}/assets/*" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" copy-editor-data)
copy_files("${CMAKE_CURRENT_SOURCE_DIR}/plugins/lua/*" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" copy-editor-plugins)

file(GLOB_RECURSE SOURCES src/*.c*)
file(GLOB_RECURSE UI src/*.ui)

add_definitions(-D__gl3w_h_ -DQT_NO_FOREACH)

if(APPLE)
	set(MACOSX_BUNDLE_ICON_FILE "neo-icon")
	set_source_files_properties(data/neo-icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

	set(MAC_RESOURCES data/neo-icon.icns)
endif()

add_executable(NeoEditor MACOSX_BUNDLE WIN32 
	${SOURCES}
	${UI}
	src/neo.qrc
	${IMGUI_SOURCES}
	${LQT_SRC} 
	${LQT_HPP}
	BreezeStyleSheets/breeze.qrc src/neo_icon.rc
	${MAC_RESOURCES})

## Add lqt as a dependency so plugins can be loaded
add_dependencies(NeoEditor qtcore qtwidgets qtgui copy-editor-plugins copy-editor-data)

add_dependencies(NeoEditor copy-editor-plugins copy-editor-data)
add_custom_target(lqt-targets ALL DEPENDS qtcore qtwidgets qtgui lfs jis gbk lrdb lpeglabel path miniz pinyin utf8)

if(WIN32)
	set(QT_WINMAIN Qt5::WinMain)
else()
	set(QT_WINMAIN "")
endif()

if(APPLE)
	set(APPLE_LUAJIT "-pagezero_size 10000 -image_base 100000000")
else()
	set(APPLE_LUAJIT "")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT APPLE)
	target_link_libraries(NeoEditor Qt5::Widgets Qt5::OpenGL Qt5::Sql ${QT_WINMAIN} Qt5::Svg "-Wl,-rpath ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" -Wl,--whole-archive NeoEngine NeoScript -Wl,--no-whole-archive)
#elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND APPLE)
#	target_link_libraries(NeoEditor "-pagezero_size 10000 -image_base 100000000" Qt5::Widgets Qt5::OpenGL Qt5::Sql ${QT_WINMAIN} Qt5::Svg "-Wl,-rpath ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" -Wl,--all_load NeoEngine NeoScript -Wl,-noall_load)
else()
	target_link_libraries(NeoEditor ${APPLE_LUAJIT} Qt5::Widgets Qt5::OpenGL Qt5::Sql ${QT_WINMAIN} Qt5::Svg NeoEngine NeoScript)
endif()

target_include_directories(NeoEditor PUBLIC src)
add_dependencies(NeoEditor build-shaders copy-editor-data lqt-targets Materials)

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
endif()

if(APPLE OR WIN32)
	install(FILES
			$<TARGET_FILE:Qt5::Widgets>
			$<TARGET_FILE:Qt5::OpenGL>
			$<TARGET_FILE:Qt5::Sql>
			$<TARGET_FILE:Qt5::Core>
			$<TARGET_FILE:Qt5::Gui>
			
			DESTINATION bin)
endif()

if(NOT APPLE)
	install(TARGETS NeoEditor
			RUNTIME DESTINATION bin
			BUNDLE DESTINATION .)
endif()

## Install lqt
if(APPLE)
	set(APPDIR "${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/")
	
	# They will be installed by copying and fixing rpath manually!
	# install(TARGETS lfs jis gbk lrdb lpeglabel path miniz pinyin utf8 qtcore qtgui qtwidgets DESTINATION ${APPDIR}/MacOS)

	# install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets DESTINATION ${APPDIR}/MacOS)
	# install(DIRECTORY plugins DESTINATION ${APPDIR}/MacOS)
else()
	install(TARGETS lfs jis gbk lrdb lpeglabel path miniz pinyin utf8 qtcore qtgui qtwidgets DESTINATION bin)

	install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets DESTINATION bin)
	install(DIRECTORY plugins DESTINATION bin FILES_MATCHING PATTERN "*" PATTERN "*Test*" EXCLUDE)
endif()

macro(install_kit name)
	if(NOT WIN32)
		install(DIRECTORY "SDK/build-linux64-${name}/linux64-${name}" DESTINATION kits/)
	else()
		install(DIRECTORY "SDK/build-win64-${name}/win64-${name}" DESTINATION kits/)
	endif()
endmacro()

if(INCLUDE_KITS)
	install_kit("shared-Release")
	install_kit("shared-Debug")
	
	if(NOT WIN32)
		install_kit("static-Release")
		install_kit("static-Debug")
	endif()

	#install(DIRECTORY "SDK/build-linux64-shared/linux64-shared-Release" DESTINATION kits/linux64-shared-release)
	#install(DIRECTORY "SDK/build-linux64-static/linux64-static-Release" DESTINATION kits/linux64-static-release)
	
	#install(DIRECTORY "SDK/build-linux64-shared/linux64-shared-Debug" DESTINATION kits/linux64-shared-debug)
	#install(DIRECTORY "SDK/build-linux64-static/linux64-static-Debug" DESTINATION kits/linux64-static-debug)
endif()

if(UNIX AND NOT APPLE)
	set(CPACK_GENERATOR "STGZ;TGZ;DEB;RPM")
	set(CPACK_STRIP_FILES OFF) ## Because the editor SDK is going to be used for development
elseif(APPLE)
	file(MAKE_DIRECTORY $<TARGET_FILE_DIR:NeoEditor>/platforms/)
	add_custom_command(TARGET NeoEditor POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:NeoEditor>/platforms/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE_DIR:Qt5::Gui>/../../plugins/platforms/libqcocoa.dylib
			$<TARGET_FILE_DIR:NeoEditor>/platforms/libqcocoa.dylib

		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE_DIR:Qt5::Gui>/../../plugins/iconengines/libqsvgicon.dylib
			$<TARGET_FILE_DIR:NeoEditor>/iconengines/libqsvgicon.dylib
		
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:lfs>
			$<TARGET_FILE:jis>
			$<TARGET_FILE:gbk>
			$<TARGET_FILE:lrdb>
			$<TARGET_FILE:lpeglabel>
			$<TARGET_FILE:path>
			$<TARGET_FILE:miniz>
			$<TARGET_FILE:pinyin>
			$<TARGET_FILE:utf8>
			$<TARGET_FILE:qtcore>
			$<TARGET_FILE:qtgui>
			$<TARGET_FILE:qtwidgets>

			$<TARGET_FILE_DIR:NeoEditor>
		
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/
			$<TARGET_FILE_DIR:NeoEditor>/assets/
		
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/
			$<TARGET_FILE_DIR:NeoEditor>/plugins/
	)

	include(InstallRequiredSystemLibraries)

	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin )
	set(APPS "\${CMAKE_CURRENT_BINARY_DIR}/bin/NeoEditor.app" ) # paths to executables
	set(DIRS "${CMAKE_CURRENT_BINARY_DIR}/lib") # directories to search for prerequisites
	set(QT_PLUGINS
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/platforms/libqcocoa.dylib
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/iconengines/libqsvgicon.dylib
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/lfs.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/jis.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/gbk.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/lrdb.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/lpeglabel.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/path.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/miniz.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/pinyin.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/utf8.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/qtcore.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/qtgui.so
		${CMAKE_BINARY_DIR}/NeoEditor.app/Contents/MacOS/qtwidgets.so
	)

	install(CODE "
		include(BundleUtilities)
		copy_and_fixup_bundle(\"${APPS}\" \"${CMAKE_BINARY_DIR}/NeoEditor.app\" \"${QT_PLUGINS}\" \"${DIRS}\")
	")

	set(CPACK_BUNDLE_NAME "Neo Editor")
	set(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/macoslaunch.sh")

#	set(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_BINARY_DIR}/bin/NeoEditor") # "${CMAKE_CURRENT_SOURCE_DIR}/scripts/macoslaunch.sh")
	set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/data/neo-icon.png")
	set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/scripts/bundle.plist")
	set(CPACK_GENERATOR "Bundle") # STGZ;TGZ")
elseif(WIN32)
	set(CPACK_GENERATOR "ZIP;NSIS")
	set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\NeoEditor.exe")
	set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Neo Editor.lnk' '\$INSTDIR\\\\bin\\\\NeoEditor.exe' ''")
	set(CPACK_NSIS_DELETE_ICONS_EXTRA "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Neo Editor.lnk' '\$INSTDIR\\\\bin\\\\NeoEditor.exe' ''")

	install(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qt.conf
			  DESTINATION bin)

	install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins
			  DESTINATION bin)

	install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
			  DESTINATION bin
			  FILES_MATCHING PATTERN "webp*.dll")

	install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
			  DESTINATION bin
			  FILES_MATCHING PATTERN "libcrypto*.dll")

	install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
			  DESTINATION bin
			  FILES_MATCHING PATTERN "icu*.dll")

	install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
			  DESTINATION bin
			  FILES_MATCHING PATTERN "pcre*.dll")

	install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
			  DESTINATION bin
			  FILES_MATCHING PATTERN "libssl*.dll")

	install(FILES
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bz2.dll
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/harfbuzz.dll
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/jasper.dll
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/jpeg62.dll
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/LIBPQ.dll
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pinyin.dll
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bz2.dll
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sqlite3.dll
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tiff.dll
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lzma.dll

			DESTINATION bin)
endif()

set(CPACK_MONOLITHIC_INSTALL 1)

set(CPACK_PACKAGE_NAME "${NEOEDITOR_PACKAGE}")
set(CPACK_PACKAGE_VENDOR "${NEOEDITOR_VENDOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neo Editor")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/data/description.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/data/licenses.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
# set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/data/neo-icon.png")
set(CPACK_PACKAGE_VERSION_MAJOR "${NEOEDITOR_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${NEOEDITOR_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${NEOEDITOR_PATCH_VERSION}")
set(CPACK_PACKAGE_CONTACT "Neo Project https://github.com/orgs/NeoGameEngineProject")

## Set install path
#set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/Neo-${NEOEDITOR_VERSION}")
#set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/Neo)

include(CPack)
