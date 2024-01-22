add_executable(tuntest tuntest.cpp)
add_executable(udpserver udpserver.cpp)

target_link_libraries(tuntest PRIVATE tun)
target_link_libraries(udpserver PRIVATE tun)

file(COPY testfile DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY test.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})