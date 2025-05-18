cmake_minimum_required(VERSION 3.30)
project(Ex3)

set(CMAKE_CXX_STANDARD 20)

add_executable(Ex3 main.cpp
        cmake-build-debug/Player.cpp
        cmake-build-debug/Player.hpp
        cmake-build-debug/Game.cpp
        cmake-build-debug/Game.hpp
        cmake-build-debug/typetools.hpp)
