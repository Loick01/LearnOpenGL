cmake_minimum_required(VERSION 3.10)
project(hello_triangle LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXTERNALS_DIR ${CMAKE_CURRENT_LIST_DIR}/../../externals)
set(INCLUDES_DIR ${CMAKE_CURRENT_LIST_DIR}/../../includes)

add_library(glad STATIC ${EXTERNALS_DIR}/glad/src/glad.c)
target_include_directories(glad PUBLIC ${EXTERNALS_DIR}/glad/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

add_executable(main src/main.cpp)

target_include_directories(main PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${GLFW_LIBRARIES} glad)
target_compile_options(main PRIVATE ${GLFW_CFLAGS_OTHER})
