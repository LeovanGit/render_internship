cmake_minimum_required(VERSION 3.3)

project(homework-1 VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC engine/src)
set(INC engine/inc)

add_executable(rt ${SRC}/engine.cpp ${SRC}/sphere.cpp ${SRC}/math.cpp)

include_directories(${INC})
