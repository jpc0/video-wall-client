add_library(renderer STATIC IndexBuffer.cpp Renderer.cpp Shader.cpp VertexArray.cpp VertexBuffer.cpp Texture.cpp)
target_include_directories(renderer PUBLIC ${CMAKE_SOURCE_DIR}/libs/Renderer)
