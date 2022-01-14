project(yamlglad LANGUAGES C)

add_library(yaml STATIC Yaml.cpp)
target_include_directories(yaml PUBLIC ${CMAKE_SOURCE_DIR}/libs/yaml)