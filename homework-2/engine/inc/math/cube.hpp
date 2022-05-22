#ifndef CUBE_HPP
#define CUBE_HPP

#include <vector>
#include "glm.hpp"

#include "triangle.hpp"
#include "matrices.hpp"

namespace math
{
class Cube
{
public:
    Cube(glm::vec3 position,
         glm::vec3 angles,
         glm::vec3 scale);

    bool intersect(math::Intersection & nearest,
                   const math::Ray & ray);

private:
    glm::mat4 model_matrix;
    glm::mat4 model_matrix_inv;
    static std::vector <math::Triangle> mesh;
};
} // namespace math

#endif
