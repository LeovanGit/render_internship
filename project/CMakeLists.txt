cmake_minimum_required(VERSION 3.3)
project(engine VERSION 1.0 LANGUAGES C CXX)

# for emacs lsp indexing
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# compilation flags
add_definitions("-Wall")

# VS multi-processor compilation
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

# enable VS folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(INC engine engine/math glm)
include_directories(${INC})

set(SOURCES engine/main.cpp
            engine/window.cpp
            engine/scene.cpp
            engine/controller.cpp
            engine/camera.cpp
            engine/material.cpp
            engine/parallel_executor.cpp)

set(MATH_SOURCES engine/math/ray.cpp
                 engine/math/intersection.cpp
                 engine/math/sphere.cpp
                 engine/math/plane.cpp
                 engine/math/triangle.cpp
                 engine/math/cube.cpp
                 engine/math/matrices.cpp
                 engine/math/euler_angles.cpp)

set(HEADERS engine/window.hpp
            engine/scene.hpp
            engine/controller.hpp
            engine/camera.hpp
            engine/material.hpp
            engine/parallel_executor.hpp)

set(MATH_HEADERS engine/math/ray.hpp
                 engine/math/intersection.hpp
                 engine/math/sphere.hpp
                 engine/math/plane.hpp
                 engine/math/triangle.hpp
                 engine/math/cube.hpp
                 engine/math/matrices.hpp
                 engine/math/euler_angles.hpp)

add_executable(rt WIN32
                  ${SOURCES} ${MATH_SOURCES}
                  ${HEADERS} ${MATH_HEADERS})

source_group("Source Files/math" FILES ${MATH_SOURCES})
source_group("Header Files/math" FILES ${MATH_HEADERS})

