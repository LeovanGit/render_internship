cmake_minimum_required(VERSION 3.3)

project(homework-2 VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC engine/src)
set(INC engine/inc)

set(GLM glm)
include_directories(${GLM})

include_directories(${INC})

add_executable(rt WIN32 ${SRC}/main.cpp 
			${SRC}/window.cpp
			${SRC}/scene.cpp
			${SRC}/controller.cpp
			${SRC}/sphere.cpp
                        ${SRC}/material.cpp
                        ${SRC}/colored_sphere.cpp
                        ${SRC}/light.cpp
                        ${SRC}/ray.cpp
                        ${SRC}/intersection.cpp
                        ${SRC}/plane.cpp)

