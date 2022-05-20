#include "object_mover.hpp"

SphereMover::SphereMover(Sphere & sphere) :
    sphere(sphere) {}

void SphereMover::move(const glm::vec3 & offset)
{
    sphere.origin += offset;
}
