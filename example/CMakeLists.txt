add_executable(normaldevice normaldevice.cpp)
add_executable(tundevice tundevice.cpp)

target_link_libraries(tundevice PRIVATE tun)
target_link_libraries(normaldevice PRIVATE tun)