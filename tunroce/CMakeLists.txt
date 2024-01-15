add_executable(RoceSend RoceSend.cpp)
add_executable(RoceRecv RoceRecv.cpp)

target_link_libraries(RoceSend PRIVATE Tun)
target_link_libraries(RoceRecv PRIVATE Tun)
file(COPY test DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
