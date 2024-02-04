add_executable(profile profile.cc)

target_link_libraries(profile PRIVATE tun)

file(COPY small DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY medium DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY large DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY runprof.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})