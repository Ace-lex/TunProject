add_executable(tuntest tuntest.cc)
add_executable(udpserver udpserver.cc)

target_link_libraries(tuntest PRIVATE tun)
target_link_libraries(udpserver PRIVATE tun)

file(COPY testfile DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY test.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})