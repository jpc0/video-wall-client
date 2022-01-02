project(glad LANGUAGES C)

add_library(glad STATIC glad.c)
target_include_directories(glad PUBLIC ${CMAKE_DL_LIBS})
target_link_libraries(glad INTERFACE -ldl)
