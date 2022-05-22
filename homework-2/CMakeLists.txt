cmake_minimum_required(VERSION 3.3)

project(homework-2 VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC engine/src)
set(INC engine/inc engine/inc/math glm)

include_directories(${INC})

add_executable(rt WIN32 ${SRC}/main.cpp 
			${SRC}/window.cpp
			${SRC}/scene.cpp
			${SRC}/controller.cpp
                        ${SRC}/camera.cpp
                        ${SRC}/material.cpp
                        ${SRC}/math/ray.cpp
                        ${SRC}/math/intersection.cpp
			${SRC}/math/sphere.cpp
                        ${SRC}/math/plane.cpp
                        ${SRC}/math/triangle.cpp
                        ${SRC}/math/cube.cpp
                        ${SRC}/math/matrices.cpp)

