cmake_minimum_required(VERSION 3.3)
project(engine VERSION 1.0 LANGUAGES C CXX)

# for emacs lsp-mode indexing
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# compilation flags
# add_definitions("-Wall")
add_definitions("/std:c++17")

# VS multi-processor compilation
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

# enable VS folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# --------------------[HEADER PATHS]--------------------
set(INC engine/source
        engine/source/math
        engine/source/render
        glm/glm
        directx_texture_loader
        spdlog/include
        spdlog/include/spdlog)

include_directories(${INC})

# --------------------[DDSTextureLoader]--------------------
set(TEX_LOADER directx_texture_loader/DDSTextureLoader11.h
               directx_texture_loader/DDSTextureLoader11.cpp)

source_group("directx_texture_loader" FILES ${TEX_LOADER})

# --------------------[SHADERS]--------------------
set(SHADERS engine/shaders/cube.hlsl
            engine/shaders/globals.hlsl
            engine/shaders/skybox.hlsl
            engine/shaders/floor.hlsl)

# does not participate in the compilation:
set_source_files_properties(${SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group("shaders" FILES ${SHADERS})

# --------------------[SOURCE]--------------------
set(HEADERS engine/source/controller.hpp
            engine/source/scene.hpp
            engine/source/camera.hpp)

set(SOURCES engine/source/controller.cpp
            engine/source/scene.cpp
            engine/source/camera.cpp
            engine/source/main.cpp)

source_group("Header Files/source" FILES ${HEADERS})
source_group("Source Files/source" FILES ${SOURCES})

# --------------------[SOURCE/RENDER]--------------------
set(RENDER_HEADERS engine/source/render/window.hpp
                   engine/source/render/dx_res_ptr.hpp
                   engine/source/render/globals.hpp
                   engine/source/render/shader.hpp
                   engine/source/render/shader_manager.hpp
                   engine/source/render/win_def.hpp
                   engine/source/render/win_undef.hpp
                   engine/source/render/texture.hpp
                   engine/source/render/texture_manager.hpp
                   engine/source/render/shader_includer.hpp
                   engine/source/render/engine.hpp
                   engine/source/render/sky.hpp)

set(RENDER_SOURCES engine/source/render/window.cpp
                   engine/source/render/globals.cpp
                   engine/source/render/shader.cpp
                   engine/source/render/shader_manager.cpp
                   engine/source/render/texture.cpp
                   engine/source/render/texture_manager.cpp
                   engine/source/render/shader_includer.cpp
                   engine/source/render/engine.cpp
                   engine/source/render/sky.cpp)

source_group("Header Files/source/render" FILES ${RENDER_HEADERS})
source_group("Source Files/source/render" FILES ${RENDER_SOURCES})

# --------------------[SOURCE/MATH]--------------------
set(MATH_HEADERS engine/source/math/matrices.hpp
                 engine/source/math/euler_angles.hpp)

set(MATH_SOURCES engine/source/math/matrices.cpp
                 engine/source/math/euler_angles.cpp)

source_group("Header Files/source/math" FILES ${MATH_HEADERS})
source_group("Source Files/source/math" FILES ${MATH_SOURCES})

# --------------------[MAIN TARGET]--------------------
add_executable(rt WIN32
                  ${HEADERS} ${SOURCES}
                  ${MATH_HEADERS} ${MATH_SOURCES}
                  ${RENDER_HEADERS} ${RENDER_SOURCES}
                  ${SHADERS}
                  ${TEX_LOADER})

# set rt in VS as startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT rt)
