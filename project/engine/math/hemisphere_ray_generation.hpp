#ifndef HEMISPHERE_RAY_GENERAION_HPP
#define HEMISPHERE_RAY_GENERAION_HPP

#include "glm.hpp"
#include "gtc/constants.hpp"

namespace math
{
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
