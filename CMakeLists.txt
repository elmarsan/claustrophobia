cmake_minimum_required(VERSION 3.22)
project(claustrophobia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

add_subdirectory(vendor/glfw)

add_executable(claustrophobia main.cpp glad.c stb_image.cpp)
target_link_libraries(claustrophobia glfw)
