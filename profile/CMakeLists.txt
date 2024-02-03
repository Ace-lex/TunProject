add_executable(profile profile.cc)

target_link_libraries(profile PRIVATE tun)

file(COPY small DESTINATION ${CMAKE_CURRENT_BINARY_DIR})