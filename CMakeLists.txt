cmake_minimum_required(VERSION 4.0)
project(temp CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)

add_executable(temp src/main.cpp)
