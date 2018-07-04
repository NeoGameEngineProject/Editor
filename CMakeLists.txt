project(TemplateGame)
cmake_minimum_required(VERSION 3.10)

set(NO_DOCS TRUE)
set(NO_TESTGAME TRUE)
set(NO_TESTS TRUE)
add_subdirectory(SDK EXCLUDE_FROM_ALL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB SOURCES src/*.c*)
add_game(TemplateGame "${SOURCES}" "./assets")
