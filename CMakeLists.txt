cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_CXX_EXTENSIONS OFF)

set(CROSSGUID_TESTS OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(_project_options URL https://github.com/cpp-best-practices/project_options/archive/refs/tags/v0.18.1.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

project(
    video_wall_client
    VERSION 0.0.1
    DESCRIPTION "A Simple video wall client"
    LANGUAGES CXX C)

project_options(
    ENABLE_CACHE
    ENABLE_CPPCHECK
    # ENABLE_CLANG_TIDY
    ENABLE_CONAN
    # ENABLE_INTERPROCEDURAL_OPTIMIZATION
    # ENABLE_NATIVE_OPTIMIZATION
    # ENABLE_DOXYGEN
    # ENABLE_COVERAGE
    # ENABLE_SANITIZER_ADDRESS
    # ENABLE_SANITIZER_LEAK
    ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
    # ENABLE_SANITIZER_THREAD
    # ENABLE_SANITIZER_MEMORY
    # ENABLE_PCH
    # PCH_HEADERS
    # WARNINGS_AS_ERRORS
    # ENABLE_INCLUDE_WHAT_YOU_USE
    # ENABLE_USER_LINKER
    # ENABLE_BUILD_WITH_TIME_TRACE
    # ENABLE_UNITY
    # CONAN_OPTIONS
    )

add_subdirectory(vendor/yaml)
add_subdirectory(vendor/crossguid)

find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(cppzmq REQUIRED)
find_package(ffmpeg REQUIRED)
find_package(nlohmann_json)
add_executable(client src/main.cpp src/Configuration.cpp src/display.cpp src/Messaging.cpp src/Video.cpp)
target_include_directories(
    client 
    PRIVATE "${CMAKE_SOURCE_DIR}/vendor/WnLSL/include"
    )
target_link_libraries(
    client
    PRIVATE project_options
    project_warnings)

target_link_system_libraries(
    client PRIVATE
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
    avutil
    nlohmann_json::nlohmann_json
)

install( TARGETS client DESTINATION bin)
install( FILES ${CMAKE_SOURCE_DIR}/config.yaml DESTINATION /etc/${PROJECT_NAME})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Packing)