
cmake_minimum_required (VERSION 2.6)
project (tiletel)

SET(CMAKE_BUILD_TYPE "Release")

SET(CMAKE_CXX_FLAGS "-Wall -std=c++11 -O4 -pthread -g")
SET(CMAKE_C_FLAGS "-Wall -O4 -pthread -g -DBUILD_ENABLE_DEBUG")

#if(WIN32)
#  SET(CMAKE_EXE_LINKER_FLAGS "-static -pthread")
#endif()


SET(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dist)

include_directories(../libtsm libtsm)

#find_program(RAGEL ragel)

#if(NOT RAGEL) 
  
#  add_executable(incavead configparser.cc incavead.cc)

#else(NOT RAGEL)

#  add_custom_command(
#    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/configparser.h
#    COMMAND ${RAGEL} -s -T0 -o configparser.h configparser.rl
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/configparser.rl
#    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#  )

#  add_executable(incavead configparser.cc incavead.cc ${CMAKE_CURRENT_SOURCE_DIR}/configparser.h)

#endif(NOT RAGEL)

add_executable(tiletel tiletel2.cc 
   libtsm/external/wcwidth.c
   libtsm/src/tsm_vte_charsets.c
   libtsm/src/shl_htable.c 
   libtsm/src/tsm_screen.c
   libtsm/src/tsm_vte.c
   libtsm/src/tsm_render.c
   libtsm/src/tsm_selection.c
   libtsm/src/tsm_unicode.c
)

if(WIN32)

  target_link_libraries(tiletel SDL SDL_net SDL_image SDL_ttf ws2_32)

else(WIN32)

  target_link_libraries(tiletel SDL2 SDL2_net SDL2_image SDL2_ttf)

endif(WIN32)

install(TARGETS tiletel DESTINATION games/tiletel)
install(FILES tiles.png DESTINATION games/tiletel)

