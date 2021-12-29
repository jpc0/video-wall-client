cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)
project(video-wall-client VERSION 0.1.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(client src/main.cpp src/Renderer.cpp src/VertexBuffer.cpp src/IndexBuffer.cpp)
target_link_libraries(client ${CONAN_LIBS})