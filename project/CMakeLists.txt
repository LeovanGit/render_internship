cmake_minimum_required(VERSION 3.3)
project(engine VERSION 1.0 LANGUAGES C CXX)

# for emacs lsp indexing
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# compilation flags
# add_definitions("-Wall")

# VS multi-processor compilation
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

set(SOURCES engine/window/main.cpp)

add_executable(rt WIN32
                  ${SOURCES})

