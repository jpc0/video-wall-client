cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
project(video-wall-client VERSION 0.1.0 LANGUAGES CXX)

message("${CMAKE_MODULE_PATH}")
find_package(SDL2 REQUIRED)
find_package(Boost 1.67 REQUIRED OPTIONAL_COMPONENTS program_options)
find_package(cppzmq REQUIRED)
add_subdirectory(libs/Renderer)
add_subdirectory(libs/glad)

add_executable(client 
    src/main.cpp 
    src/stbimage.cpp
    src/Configuration.cpp
    src/Display.cpp
    )
target_link_libraries(client 
    cppzmq
    glad
    renderer 
    SDL2main
    SDL2 
    Boost::program_options
    ${CMAKE_DL_LIBS})
target_include_directories(client PUBLIC 
    ${CMAKE_SOURCE_DIR}/libs/Renderer 
    ${CMAKE_SOURCE_DIR}/include 
    ${SDL_INCLUDE_DIRS} 
    ${Boost_INCLUDE_DIRS}
    )

install( TARGETS client DESTINATION bin)
install( FILES ${CMAKE_SOURCE_DIR}/config.cfg DESTINATION /etc/${PROJECT_NAME})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Packing)