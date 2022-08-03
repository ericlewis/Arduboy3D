cmake_minimum_required(VERSION 3.14)
set (CMAKE_CXX_STANDARD 11)
add_definitions(-D_PLAYDATE)

if (APPLE)
    execute_process(
            COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
            COMMAND head -n 1
            COMMAND cut -c9-
            OUTPUT_VARIABLE SDK
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(LIB_EXTENSION dylib)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")
    set(SDK $ENV{PLAYDATE_SDK_PATH})
    set(LIB_EXTENSION dll)
endif()

set(PLAYDATE_GAME_NAME Catacombs)
set(PLAYDATE_GAME_DEVICE Catacombs)

project(${PLAYDATE_GAME_NAME} C CXX ASM)

include_directories(
    src
    ../Arduboy3D
)

set(SOURCE
    src/PlaydateMain.cpp

    ../Arduboy3D/Draw.cpp
    ../Arduboy3D/FixedMath.cpp
    ../Arduboy3D/MapGenerator.cpp
    ../Arduboy3D/Game.cpp
    ../Arduboy3D/Map.cpp
    ../Arduboy3D/Enemy.cpp
    ../Arduboy3D/Entity.cpp
    ../Arduboy3D/Particle.cpp
    ../Arduboy3D/Menu.cpp
    ../Arduboy3D/Player.cpp
    ../Arduboy3D/Projectile.cpp
    ../Arduboy3D/Sounds.cpp
    ../Arduboy3D/Font.cpp
)

if (TOOLCHAIN STREQUAL "armgcc")
    add_definitions(-D_PLAYDATE_DEVICE)
    set(PLAYDATE_TARGET_NAME ${PLAYDATE_GAME_NAME})
    add_executable(${PLAYDATE_TARGET_NAME} ${SDK}/C_API/buildsupport/setup.c ${SOURCE})
else()
    set(PLAYDATE_TARGET_NAME ${PLAYDATE_GAME_NAME})
    add_library(${PLAYDATE_TARGET_NAME} SHARED ${SOURCE})
endif()

set_property(TARGET ${PLAYDATE_TARGET_NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${PLAYDATE_GAME_NAME}.pdx")
include(${CMAKE_CURRENT_SOURCE_DIR}/.vscode/playdate_game.patched.cmake)



