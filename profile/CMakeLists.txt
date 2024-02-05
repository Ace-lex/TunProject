add_executable(profile profile.cc)

target_link_libraries(profile PRIVATE tun)

file(GLOB profile_folders "./prof*")
foreach(folder ${profile_folders})
    file(COPY ${folder} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
file(COPY runprof.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})