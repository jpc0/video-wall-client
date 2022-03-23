find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(cppzmq REQUIRED)
find_package(ffmpeg REQUIRED)
add_executable(client main.cpp Configuration.cpp display.cpp Messaging.cpp Video.cpp)
target_link_libraries(
    client
    PRIVATE project_options
    project_warnings
    fmt::fmt
    spdlog::spdlog
    SDL2
    SDL2_image
    crossguid
    yaml
    cppzmq-static
    avformat
    avcodec
    swscale
    avutil)
