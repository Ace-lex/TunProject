add_executable(rocesend rocesend.cpp)
add_executable(rocerecv rocerecv.cpp)

target_link_libraries(rocesend PRIVATE tun)
target_link_libraries(rocerecv PRIVATE tun)
file(COPY test DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
