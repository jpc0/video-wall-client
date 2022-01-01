cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
project(video-wall-client VERSION 0.1.0)
add_subdirectory(libs/Renderer)

add_executable(client src/main.cpp src/stbimage.cpp)
target_link_libraries(client ${CONAN_LIBS} renderer)
target_include_directories(client PUBLIC ${CMAKE_SOURCE_DIR}/libs/Renderer ${CMAKE_SOURCE_DIR}/include)