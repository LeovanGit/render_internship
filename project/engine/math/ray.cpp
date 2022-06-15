#include "ray.hpp"

namespace
{
    const float PI = atanf(1.0f) * 4.0f;
    const float GOLDEN_RATIO = (1.0f + sqrtf(5.0f)) / 2.0f;
} // namespace

math::Ray::Ray(const glm::vec3 & origin,
               const glm::vec3 & direction) :
               origin(origin),
               direction(direction)
{}

void math::onb_frisvad(const glm::vec3 & normal,
                       glm::vec3 & b1,
                       glm::vec3 & b2)
{
    float sign = copysignf(1.0f, normal.z);
    const float a = -1.0f / (sign + normal.z);
    const float b = normal.x * normal.y * a;
    b1 = glm::vec3(1.0f + sign * normal.x * normal.x * a,
                   sign * b,
                   -sign * normal.x);
    b2 = glm::vec3(b,
                   sign + normal.y * normal.y * a,
                   -normal.y);
}

// generate random point on unit hemisphere
glm::vec3 math::generate_point_on_hemisphere(int sample,
                                             int samples_count)
{
    // for sphere
    // float theta = acosf(1.0f - 2.0f * sample / samples_count);
    // float phi = 2 * PI * sample / GOLDEN_RATIO;

    // for hemisphere
    float theta = acosf(1.0f - (2.0f * sample + 1.0f) / (2.0f * samples_count));
    float phi = 2 * PI * sample / GOLDEN_RATIO;

    // convert spherical to Cartesian coordinates, r = 1
    glm::vec3 point(cos(phi) * sin(theta),
                    sin(phi) * sin(theta),
                    cos(theta));

    // !!! in spherical coordinates: Up = z, Right = x, Forward = -y
    // !!! in engine coordinates: Up = y, Right = x, Forward = z
    return glm::vec3(point.x,
                     point.z,
                     -point.y);
}
