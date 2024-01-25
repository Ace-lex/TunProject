add_executable(rocesend rocesend.cc)
add_executable(rocerecv rocerecv.cc)

target_link_libraries(rocesend PRIVATE tun)
target_link_libraries(rocerecv PRIVATE tun)
file(COPY payload DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
