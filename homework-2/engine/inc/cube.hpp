#ifndef CUBE_HPP
#define CUBE_HPP

#include "triangle.hpp"
#include "material.hpp"
#include "matrices.hpp"

#include <vector>

class Cube
{
public:
    Cube(glm::vec3 position,
         glm::vec3 angles,
         glm::vec3 scale,
         Material material);

    bool intersect(Intersection & nearest, Ray & ray);

    Material material;

private:
    glm::mat4 model_matrix;
    glm::mat4 model_matrix_inv;
    static std::vector <Triangle> mesh;
};

#endif
