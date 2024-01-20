add_executable(NormalDevice NormalDevice.cpp)
add_executable(TunDevice TunDevice.cpp)

target_link_libraries(TunDevice PRIVATE Tun)
target_link_libraries(NormalDevice PRIVATE Tun)