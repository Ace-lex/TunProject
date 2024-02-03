cmake_minimum_required(VERSION 3.9)
project(Tun)

add_subdirectory(libtun)
add_subdirectory(example)
add_subdirectory(test)
add_subdirectory(tunroce)
add_subdirectory(profile)

file(COPY script.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})