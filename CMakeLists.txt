cmake_minimum_required(VERSION 3.9)
project(Tun)

add_subdirectory(libTun)
add_subdirectory(example)
add_subdirectory(test)
add_subdirectory(tunroce)

file(COPY script.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})