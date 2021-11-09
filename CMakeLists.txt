cmake_minimum_required(VERSION 3.20.5)
project(unsilence_3000 VERSION 0.1)

add_subdirectory(lib/FFmpeg)

list(APPEND SOURCES
    src/main.cpp
    src/streams_divider.cpp
    src/streams_divider.hpp
)

add_executable(unsilence_3000 ${SOURCES})

target_link_libraries(unsilence_3000 FFmpeg)