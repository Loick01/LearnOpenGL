cmake_minimum_required(VERSION 3.10)
project(mesh_loading LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXTERNALS_DIR ${CMAKE_CURRENT_LIST_DIR}/../externals)
set(INCLUDES_DIR ${CMAKE_CURRENT_LIST_DIR}/../includes)

add_library(glad STATIC ${EXTERNALS_DIR}/glad/src/glad.c)
target_include_directories(glad PUBLIC ${EXTERNALS_DIR}/glad/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

add_subdirectory(${EXTERNALS_DIR}/glm ${CMAKE_BINARY_DIR}/glm)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_CLI OFF CACHE BOOL "" FORCE)
add_subdirectory(${EXTERNALS_DIR}/assimp ${CMAKE_BINARY_DIR}/assimp)

add_executable(main src/main.cpp ${INCLUDES_DIR}/stb_image.cpp)

target_include_directories(main PRIVATE ${GLFW_INCLUDE_DIRS} ${INCLUDES_DIR})
target_link_libraries(main PRIVATE ${GLFW_LIBRARIES} glad glm assimp)
target_compile_options(main PRIVATE ${GLFW_CFLAGS_OTHER})
