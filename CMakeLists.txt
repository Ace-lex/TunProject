cmake_minimum_required(VERSION 3.9)
project(Tun)

add_subdirectory(libTun)
add_subdirectory(example)
add_subdirectory(Test)
add_subdirectory(TunRoce)

file(COPY script.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})