add_library(renderer STATIC IndexBuffer.cpp Renderer.cpp Shader.cpp VertexArray.cpp VertexBuffer.cpp Texture.cpp)
target_link_libraries(renderer PUBLIC glad)
target_include_directories(renderer PUBLIC ${CMAKE_SOURCE_DIR}/libs/Renderer ${CMAKE_SOURCE_DIR}/include)
