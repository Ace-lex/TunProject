add_executable(RoceSend RoceSend.cpp)
add_executable(RecvRoce RecvRoce.cpp)

target_link_libraries(RoceSend PRIVATE Tun)
target_link_libraries(RecvRoce PRIVATE Tun)
file(COPY test DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
