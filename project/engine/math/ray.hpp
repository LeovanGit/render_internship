#ifndef RAY_HPP
#define RAY_HPP

#include "glm.hpp"

namespace math
{
class Ray
{
public:
    Ray() = default;

    Ray(const glm::vec3 & origin,
        const glm::vec3 & direction);

    glm::vec3 reflect(const glm::vec3 & normal) const;

glm::vec3 origin;
glm::vec3 direction;
};

inline glm::vec3 reflect(const glm::vec3 & ray,
                         const glm::vec3 & normal)
{
    return 2.0f * glm::dot(ray, normal) * normal - ray;
}

// Building an Orthonormal Basis
void onb_frisvad(const glm::vec3 & normal,
                 glm::vec3 & b1,
                 glm::vec3 & b2);

// Spherical Fibonacci Point Sets
// Fibonacci generate points over all sphere
// this function a little modified for hemisphere case
glm::vec3 generate_point_on_hemisphere(int sample,
                                       int samples_count);
} // namespace math

#endif

