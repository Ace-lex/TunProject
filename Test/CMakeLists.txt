add_executable(TunTest TunTest.cpp)
add_executable(UDPServer UDPServer.cpp)

target_link_libraries(TunTest PRIVATE Tun)
target_link_libraries(UDPServer PRIVATE Tun)

file(COPY test0 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY test1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY test2 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY test.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})