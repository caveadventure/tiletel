
cmake_minimum_required (VERSION 2.6)
project (tiletel)

SET(CMAKE_BUILD_TYPE "Release")

if(TOOLKIT STREQUAL "Qt")

  find_package(Qt5Gui)
  find_package(Qt5Network)

  set(TILETEL_SRC tiletel-qt.cc)

elseif(TOOLKIT STREQUAL "web")

  set(TILETEL_SRC tiletel-web.cc sha1.cc)

else()

  set(TILETEL_SRC tiletel-sdl.cc)

endif()

SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -std=c++11 -O4 -pthread -g ")
SET(CMAKE_C_FLAGS_RELEASE "-Wall -O4 -pthread -g")

if(WIN32)

  SET(CMAKE_EXE_LINKER_FLAGS " -static -mwindows")

else(WIN32)

  ADD_DEFINITIONS(-DHAVE_PTY_SUPPORT)

endif()



SET(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dist)

include_directories(../libtsm libtsm)

find_program(RAGEL ragel)

if(NOT RAGEL) 
  
  add_executable(tiletel ${TILETEL_SRC}
     libtsm/external/wcwidth.c
     libtsm/src/tsm_vte_charsets.c
     libtsm/src/shl_htable.c 
     libtsm/src/tsm_screen.c
     libtsm/src/tsm_vte.c
     libtsm/src/tsm_render.c
     libtsm/src/tsm_selection.c
     libtsm/src/tsm_unicode.c
  )

else(NOT RAGEL)

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/bdf.h
    COMMAND ${RAGEL} -s -G2 -o bdf.h bdf.rl
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/bdf.rl
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/config.h
    COMMAND ${RAGEL} -s -G2 -o config.h config.rl
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.rl
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

  add_executable(tiletel ${TILETEL_SRC}
     libtsm/external/wcwidth.c
     libtsm/src/tsm_vte_charsets.c
     libtsm/src/shl_htable.c 
     libtsm/src/tsm_screen.c
     libtsm/src/tsm_vte.c
     libtsm/src/tsm_render.c
     libtsm/src/tsm_selection.c
     libtsm/src/tsm_unicode.c
     ${CMAKE_CURRENT_SOURCE_DIR}/bdf.h
     ${CMAKE_CURRENT_SOURCE_DIR}/config.h
   )

endif(NOT RAGEL)


if(WIN32)

  if(TOOLKIT STREQUAL "Qt")

    target_link_libraries(tiletel 
                          mingw32 Qt5::Gui Qt5::Network ws2_32 winmm imm32 version iphlpapi)

  else(TOOLKIT STREQUAL "Qt")

    target_link_libraries(tiletel 
                          mingw32 SDL2main SDL2 SDL2_net SDL2_image ws2_32 winmm imm32 version iphlpapi)

  endif(TOOLKIT STREQUAL "Qt")

else(WIN32)

  if(TOOLKIT STREQUAL "Qt")

    target_link_libraries(tiletel Qt5::Gui Qt5::Network util)

  elseif(TOOLKIT STREQUAL "web")

    target_link_libraries(tiletel gd util)

  else(TOOLKIT STREQUAL "Qt")

    target_link_libraries(tiletel SDL2 SDL2_net SDL2_image util)

  endif(TOOLKIT STREQUAL "Qt")

endif(WIN32)

#install(TARGETS tiletel DESTINATION games/tiletel)
#install(FILES tiles.png DESTINATION games/tiletel)

