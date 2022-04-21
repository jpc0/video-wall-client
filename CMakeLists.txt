cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_EXTENSIONS OFF)

set(CROSSGUID_TESTS OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(_project_options URL https://github.com/cpp-best-practices/project_options/archive/refs/tags/v0.18.1.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 6ea2dec23c3df14ac3b27b7d2d6bbff0cb7ba1b0)
FetchContent_MakeAvailable(cpr)

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
    # ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
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
find_package(SDL2_image CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(argh CONFIG REQUIRED)
add_executable(client 
    src/main.cpp 
    src/Configuration.cpp 
    src/display.cpp 
    src/Command.cpp 
    src/Video.cpp
    src/MessageHandler/MessageHandler.cpp
    )

target_include_directories(
    client 
    PRIVATE "${CMAKE_SOURCE_DIR}/vendor/WnLSL/include"
    )

message("${CONAN_INCLUDE_DIRS_ARGH}")
target_link_libraries(
        client
        PRIVATE project_options
        SDL2main
        SDL2
        SDL2_image
        argh::argh
        avcodec
        avfilter
        avformat
        swscale
        avutil
        project_warnings)

target_link_system_libraries(
    client PRIVATE
    fmt::fmt
    spdlog::spdlog
    crossguid
    yaml
    cppzmq::cppzmq
    nlohmann_json::nlohmann_json
    cpr::cpr
)

install( TARGETS client DESTINATION bin)
install( FILES ${CMAKE_SOURCE_DIR}/config.yaml DESTINATION /etc/${PROJECT_NAME})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Packing)