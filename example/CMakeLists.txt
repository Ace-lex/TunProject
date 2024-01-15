add_executable(NormalDevice NormalDevice.cpp)
add_executable(TunDevice TunDevice.cpp)

target_link_libraries(TunDevice PRIVATE Tun)
target_link_libraries(NormalDevice PRIVATE Tun)

file(COPY Mes1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Mes2 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})