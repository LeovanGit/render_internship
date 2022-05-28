#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <cmath>
#include "glm.hpp"

#include "ray.hpp"
#include "intersection.hpp"
#include "euler_angles.hpp"

namespace math
{
class Triangle
{
public:
    Triangle(const glm::vec3 & normal,
             const glm::vec3 & vertex_1,
             const glm::vec3 & vertex_2,
             const glm::vec3 & vertex_3);

    bool intersect(Intersection & nearest, const Ray & ray) const;

    glm::vec3 normal;
    glm::vec3 vertex_1;
    glm::vec3 vertex_2;
    glm::vec3 vertex_3;
};
} // namespace math

#endif
