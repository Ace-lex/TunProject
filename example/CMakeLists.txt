add_executable(normaldevice normaldevice.cc)
add_executable(tundevice tundevice.cc)

target_link_libraries(tundevice PRIVATE tun)
target_link_libraries(normaldevice PRIVATE tun)