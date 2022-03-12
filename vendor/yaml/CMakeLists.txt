project(yaml)

add_library(yaml STATIC Yaml.cpp)
target_include_directories(yaml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)