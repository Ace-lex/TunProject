add_library(tun SHARED tun.cc)

message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(tun PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)