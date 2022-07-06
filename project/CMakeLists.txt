cmake_minimum_required(VERSION 3.3)
project(engine VERSION 1.0 LANGUAGES C CXX)

# for emacs lsp indexing
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# compilation flags
# add_definitions("-Wall")
add_definitions("/std:c++17")

# VS multi-processor compilation
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

# enable VS folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(INC engine engine/math engine/render glm)
include_directories(${INC})

set(SHADERS engine/shaders/shader.hlsl)
set_source_files_properties(${SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(SOURCES engine/main.cpp
            engine/scene.cpp
            engine/controller.cpp
            engine/camera.cpp
            engine/material.cpp
            engine/parallel_executor.cpp)

set(RENDER_SOURCES engine/render/window.cpp
                   engine/render/globals.cpp
                   engine/render/shader.cpp)

set(MATH_SOURCES engine/math/ray.cpp
                 engine/math/intersection.cpp
                 engine/math/sphere.cpp
                 engine/math/plane.cpp
                 engine/math/triangle.cpp
                 engine/math/cube.cpp
                 engine/math/matrices.cpp
                 engine/math/euler_angles.cpp
                 engine/math/hemisphere_ray_generation.cpp)

set(HEADERS engine/scene.hpp
            engine/controller.hpp
            engine/camera.hpp
            engine/material.hpp
            engine/parallel_executor.hpp)

set(RENDER_HEADERS engine/render/window.hpp
                   engine/render/dx_res_ptr.hpp
                   engine/render/globals.hpp
                   engine/render/shader.hpp
                   engine/render/win_def.hpp
                   engine/render/win_undef.hpp)

set(MATH_HEADERS engine/math/ray.hpp
                 engine/math/intersection.hpp
                 engine/math/sphere.hpp
                 engine/math/plane.hpp
                 engine/math/triangle.hpp
                 engine/math/cube.hpp
                 engine/math/matrices.hpp
                 engine/math/euler_angles.hpp
                 engine/math/hemisphere_ray_generation.hpp
                 engine/math/constants.hpp)

#add_executable(rt WIN32
#                  ${HEADERS} ${SOURCES}
#                  ${MATH_HEADERS} ${MATH_SOURCES}
#                  ${RENDER_HEADERS} ${RENDER_SOURCES})

add_executable(rt WIN32
                  engine/main.cpp
                  ${RENDER_HEADERS} ${RENDER_SOURCES}
                  ${SHADERS})

source_group("Source Files/math" FILES ${MATH_SOURCES})
source_group("Header Files/math" FILES ${MATH_HEADERS})

source_group("Source Files/render" FILES ${RENDER_SOURCES})
source_group("Header Files/render" FILES ${RENDER_HEADERS})

source_group("shaders" FILES ${SHADERS})