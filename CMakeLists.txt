cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
project(video-wall-client VERSION 0.1.0 LANGUAGES CXX)

set(CPPZMQ_BUILD_TESTS False)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

message("${CMAKE_MODULE_PATH}")
# find_package(SDL2 REQUIRED)
# find_package(cppzmq REQUIRED)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/yaml)
add_subdirectory(vendor/libzmq)
add_subdirectory(vendor/cppzmq)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/glfw)
add_subdirectory(libs/Renderer)

add_executable(client 
    src/main.cpp 
    src/stbimage.cpp
    src/Configuration.cpp
    src/Display.cpp
    src/Platform/Linux/LinuxWindow.cpp
    src/Platform/OpenGL/OpenGLContext.cpp
    src/Layer.cpp
    src/LayerStack.cpp
    )
target_link_libraries(client
    glfw 
    yaml
    cppzmq
    glad
    glm
    renderer 
    ${CMAKE_DL_LIBS})
target_include_directories(client PUBLIC 
    ${CMAKE_SOURCE_DIR}/libs/Renderer 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/Platform/Linux
    ${CMAKE_SOURCE_DIR}/src/Platform/OpenGL
    ${SDL_INCLUDE_DIRS} 
    /usr/local/include
    )

install( TARGETS client DESTINATION bin)
install( FILES ${CMAKE_SOURCE_DIR}/config.yaml DESTINATION /etc/${PROJECT_NAME})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Packing)