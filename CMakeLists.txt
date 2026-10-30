cmake_minimum_required(VERSION 3.16)
project(FpsGame C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download raylib automatically the first time you build
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
)
FetchContent_MakeAvailable(raylib)

add_executable(fps_game
    src/main.cpp
    src/Player.cpp
    src/Weapon.cpp
    src/Level.cpp
    src/Hud.cpp
)
target_link_libraries(fps_game PRIVATE raylib)
