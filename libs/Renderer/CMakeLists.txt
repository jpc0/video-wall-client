project(renderer)

add_library(renderer STATIC IndexBuffer.cpp Renderer.cpp Shader.cpp VertexArray.cpp VertexBuffer.cpp Texture.cpp)
target_link_libraries(renderer
    glad 
    yaml 
    crossguid)
target_include_directories(renderer PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/include
    /usr/local/include)
