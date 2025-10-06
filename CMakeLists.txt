cmake_minimum_required(VERSION 3.30)
project(Challenge)

set(CMAKE_CXX_STANDARD 23)


include_directories(include)

add_executable(Challenge main.cpp sharedData.hpp)
