add_library(renderer STATIC IndexBuffer.cpp Renderer.cpp Shader.cpp VertexArray.cpp VertexBuffer.cpp Texture.cpp)
target_link_libraries(renderer PUBLIC glad)
target_include_directories(renderer PUBLIC 
    ${CMAKE_SOURCE_DIR}/vendor/yaml 
    ${CMAKE_SOURCE_DIR}/libs/Renderer 
    ${CMAKE_SOURCE_DIR}/libs/glad 
    ${CMAKE_SOURCE_DIR}/libs/yaml 
    ${CMAKE_SOURCE_DIR}/include
    /usr/local/include)
