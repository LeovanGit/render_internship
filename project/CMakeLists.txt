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

set(INC engine
        engine/math
        engine/render
        glm
        directx_texture_loader)

include_directories(${INC})

set(SHADERS engine/shaders/shader.hlsl)

set(HEADERS engine/controller.hpp
            engine/camera.hpp)

set(SOURCES engine/main.cpp
            engine/controller.cpp
            engine/camera.cpp)

set(RENDER_HEADERS engine/render/window.hpp
                   engine/render/dx_res_ptr.hpp
                   engine/render/globals.hpp
                   engine/render/shader.hpp
                   engine/render/win_def.hpp
                   engine/render/win_undef.hpp
                   directx_texture_loader/DDSTextureLoader11.h
                   engine/render/texture_manager.hpp)

set(RENDER_SOURCES engine/render/window.cpp
                   engine/render/globals.cpp
                   engine/render/shader.cpp
                   directx_texture_loader/DDSTextureLoader11.cpp
                   engine/render/texture_manager.cpp)

set(MATH_HEADERS engine/math/matrices.hpp
                 engine/math/euler_angles.hpp)

set(MATH_SOURCES engine/math/matrices.cpp
                 engine/math/euler_angles.cpp)

add_executable(rt WIN32
                  ${HEADERS} ${SOURCES}
                  ${MATH_HEADERS} ${MATH_SOURCES}
                  ${RENDER_HEADERS} ${RENDER_SOURCES}
                  ${SHADERS})

# does not participate in the compilation
set_source_files_properties(${SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group("Source Files/math" FILES ${MATH_SOURCES})
source_group("Header Files/math" FILES ${MATH_HEADERS})

source_group("Source Files/render" FILES ${RENDER_SOURCES})
source_group("Header Files/render" FILES ${RENDER_HEADERS})

source_group("shaders" FILES ${SHADERS})