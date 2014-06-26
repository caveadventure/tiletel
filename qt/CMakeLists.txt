cmake_minimum_required(VERSION 2.8.11)

project(tezt)

SET(CMAKE_BUILD_TYPE "Release")

SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -std=c++11 -O4 -pthread -g")

include_directories(../ ../libtsm libtsm)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Gui)

add_executable(tezt qt.cc
     ../libtsm/external/wcwidth.c
     ../libtsm/src/tsm_vte_charsets.c
     ../libtsm/src/shl_htable.c 
     ../libtsm/src/tsm_screen.c
     ../libtsm/src/tsm_vte.c
     ../libtsm/src/tsm_render.c
     ../libtsm/src/tsm_selection.c
     ../libtsm/src/tsm_unicode.c
)

target_link_libraries(tezt Qt5::Gui)
