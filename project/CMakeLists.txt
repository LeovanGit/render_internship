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
        spdlog/include/spdlog
        assimp/include
        build/assimp/include # config.h appears only after compilation
      )

include_directories(${INC})

# --------------------[DDSTextureLoader]--------------------
set(TEX_LOADER directx_texture_loader/DDSTextureLoader11.h
               directx_texture_loader/DDSTextureLoader11.cpp)

source_group("directx_texture_loader" FILES ${TEX_LOADER})

# --------------------[SHADERS]--------------------
set(SHADERS engine/shaders/opaque.hlsl
            engine/shaders/emissive.hlsl
            engine/shaders/globals.hlsl
            engine/shaders/skybox.hlsl
            engine/shaders/resolve.hlsl
            engine/shaders/lighting.hlsl
            engine/shaders/shadow_cubemap.hlsl)

# does not participate in the compilation:
set_source_files_properties(${SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group("shaders" FILES ${SHADERS})

# --------------------[SOURCE]--------------------
set(HEADERS engine/source/controller.hpp
            engine/source/scene.hpp
            engine/source/camera.hpp
            engine/source/transform_system.hpp)

set(SOURCES engine/source/controller.cpp
            engine/source/scene.cpp
            engine/source/camera.cpp
            engine/source/main.cpp
            engine/source/transform_system.cpp)

source_group("Header Files/source" FILES ${HEADERS})
source_group("Source Files/source" FILES ${SOURCES})

# --------------------[SOURCE/RENDER]--------------------
set(RENDER_HEADERS engine/source/render/window.hpp
                   engine/source/render/win_def.hpp
                   engine/source/render/win_undef.hpp
                   engine/source/render/dx_res_ptr.hpp
                   engine/source/render/globals.hpp
                   engine/source/render/shader.hpp
                   engine/source/render/shader_manager.hpp
                   engine/source/render/texture.hpp
                   engine/source/render/texture_manager.hpp
                   engine/source/render/shader_includer.hpp
                   engine/source/render/engine.hpp
                   engine/source/render/sky.hpp
                   engine/source/render/model.hpp
                   engine/source/render/model_manager.hpp
                   engine/source/render/vertex_buffer.hpp
                   engine/source/render/index_buffer.hpp
                   engine/source/render/opaque_instances.hpp
                   engine/source/render/emissive_instances.hpp
                   engine/source/render/mesh_system.hpp
                   engine/source/render/light_system.hpp
                   engine/source/render/vertex.hpp
                   engine/source/render/post_process.hpp)

set(RENDER_SOURCES engine/source/render/window.cpp
                   engine/source/render/globals.cpp
                   engine/source/render/shader.cpp
                   engine/source/render/shader_manager.cpp
                   engine/source/render/texture.cpp
                   engine/source/render/texture_manager.cpp
                   engine/source/render/shader_includer.cpp
                   engine/source/render/engine.cpp
                   engine/source/render/sky.cpp
                   engine/source/render/model.cpp
                   engine/source/render/model_manager.cpp
                   engine/source/render/index_buffer.cpp
                   engine/source/render/opaque_instances.cpp
                   engine/source/render/emissive_instances.cpp
                   engine/source/render/mesh_system.cpp
                   engine/source/render/light_system.cpp
                   engine/source/render/post_process.cpp)

source_group("Header Files/source/render" FILES ${RENDER_HEADERS})
source_group("Source Files/source/render" FILES ${RENDER_SOURCES})

# --------------------[SOURCE/MATH]--------------------
set(MATH_HEADERS engine/source/math/matrices.hpp
                 engine/source/math/euler_angles.hpp
                 engine/source/math/matrices.hpp
                 engine/source/math/constants.hpp
                 engine/source/math/box.hpp
                 engine/source/math/solid_vector.hpp
                 engine/source/math/triangle_octree.hpp
                 engine/source/math/ray.hpp
                 engine/source/math/mesh_intersection.hpp)

set(MATH_SOURCES engine/source/math/matrices.cpp
                 engine/source/math/euler_angles.cpp
                 engine/source/math/matrices.cpp
                 engine/source/math/triangle_octree.cpp
                 engine/source/math/ray.cpp)

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

# ------------------[ASSIMP COMPILATION]---------------
# check if assimp directory is empty
file(GLOB DIR assimp/*)
list(LENGTH DIR LEN)
if (LEN EQUAL 0)
  message("\n====================[WARNING]====================\n"
          "Assimp directory is empty.\n"
          "Clone it from github: git submodule update --init --recursive\n"
          "or set up assimp paths by yourself in the IDE.\n"
          "===================================================\n")
else()
  add_subdirectory(assimp)
  target_link_libraries(rt assimp)

  # copy .dll to .exe directory (post-build event)
  # $<CONFIGURATION> means Debug/Release directory
  # depends on cmake --build . --config Debug/Release
  add_custom_command(TARGET rt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/build/assimp/bin/$<CONFIGURATION>"
    "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
endif()
