cmake_minimum_required(VERSION 3.12.1 FATAL_ERROR)
project(clipshort VERSION 1.0.0 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH

    "${CMAKE_SOURCE_DIR}/cmake"   # FindLibClipboard.cmake
    "/usr/share/ECM/find-modules" # FindXCB required by libclipboard
)

set(CMAKE_C_FLAGS     "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow")
set(CMAKE_C_STANDARD  11)

add_subdirectory("src")
