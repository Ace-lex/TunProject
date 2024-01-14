add_library(Tun SHARED Tun.cpp)

message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(Tun PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)