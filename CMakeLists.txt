cmake_minimum_required(VERSION 3.16)

project(Starliner)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(fmt REQUIRED)
find_package(assimp REQUIRED)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/*.c"
)

add_executable(Starliner ${SOURCE_FILES})

target_include_directories(Starliner PRIVATE
    src
)

target_include_directories(Starliner SYSTEM PRIVATE
    Libraries/include
)

target_link_libraries(Starliner PRIVATE
    OpenGL::GL
    glfw
    fmt::fmt
    assimp::assimp
)

target_compile_options(Starliner PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -fsanitize=address
    -fsanitize=undefined
    -g
)

target_link_options(Starliner PRIVATE
    -fsanitize=address
    -fsanitize=undefined
)

set(PEDANTIC_EXCEPTIONS
    src/glad.c
)

set_source_files_properties(${PEDANTIC_EXCEPTIONS} 
    PROPERTIES COMPILE_OPTIONS "-Wno-pedantic"
)
