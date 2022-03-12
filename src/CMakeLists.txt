find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(SFML COMPONENTS window graphics network audio REQUIRED)
find_package(cppzmq)
add_executable(client main.cpp Configuration.cpp display.cpp)
target_link_libraries(
    client
    PRIVATE project_options
    project_warnings
    fmt::fmt
    spdlog::spdlog
    sfml-graphics
    crossguid
    yaml
    cppzmq-static)
