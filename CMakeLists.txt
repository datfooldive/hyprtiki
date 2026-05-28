cmake_minimum_required(VERSION 3.27)
project(hyprtiki
    DESCRIPTION "Hyprland plugin to play embedded background music on loop"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(HYPRLAND REQUIRED hyprland)

add_subdirectory(src)
