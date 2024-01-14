add_executable(UDPSend UDPSend.cpp)
add_executable(TunRecvSend TunRecvSend.cpp)

target_link_libraries(TunRecvSend PRIVATE Tun)
target_link_libraries(UDPSend PRIVATE Tun)

file(COPY Mes1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Mes2 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})