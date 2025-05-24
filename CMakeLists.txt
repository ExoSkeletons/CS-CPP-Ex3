cmake_minimum_required(VERSION 3.30)
project(Ex3)

set(CMAKE_CXX_STANDARD 20)

add_executable(Ex3 main.cpp
        player.cpp
        player.hpp
        game.cpp
        game.hpp
        typetools.hpp
        GameActions.cpp
        GameActions.hpp)
