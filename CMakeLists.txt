cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(clipshort VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS} -pedantic -Wall -Wextra -Wshadow")
set(CMAKE_C_STANDARD 11)

add_subdirectory("src")
