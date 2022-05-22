#include "cube.hpp"
#include "matrix.hpp"

math::Cube::Cube(glm::vec3 position,
                 glm::vec3 angles,
                 glm::vec3 scale)
{
    Transform t(position, angles, scale);
    model_matrix = t.toMat4();
    model_matrix_inv = glm::inverse(model_matrix);
}

bool math::Cube::intersect(math::Intersection & nearest,
                           const math::Ray & ray)
{
    bool found_intersection = false;

    // transform ray to Model Space
    glm::vec4 orig_ms = model_matrix_inv * glm::vec4(ray.origin, 1.0f);
    glm::vec4 dir_ms = model_matrix_inv * glm::vec4(ray.direction, 0.0f);

    Ray ray_ms(glm::vec3(orig_ms) / orig_ms.w,
               glm::normalize(glm::vec3(dir_ms)));

    // find nearest from all mesh in Model Space
    Intersection nearest_ms;
    nearest_ms.reset();
   
    for (int i = 0, size = mesh.size(); i != size; ++i)
    {
        found_intersection |= mesh[i].intersect(nearest_ms, ray_ms);
    }
    if (!found_intersection) return false; // no intersection

    // back to World Space
    glm::vec4 point_ws = model_matrix * glm::vec4(nearest_ms.point, 1.0f);
    glm::vec3 point_ws_norm = glm::vec3(point_ws) / point_ws.w;

    float t = glm::length(point_ws_norm - ray.origin);

    if (t >= nearest.t) return false; // not nearest intersection

    glm::vec4 normal_ws = model_matrix * glm::vec4(nearest_ms.normal, 0.0f);

    nearest.t = t;
    nearest.point = point_ws_norm;
    nearest.normal = glm::normalize(glm::vec3(normal_ws));

    return true;
}

// create one mesh for all cubes
std::vector <math::Triangle> math::Cube::mesh = {
    Triangle(glm::vec3(0, 0, -1.0f),
             glm::vec3(-1.0f, -1.0f, -1.0f),
             glm::vec3(-1.0f, 1.0f, -1.0f),
             glm::vec3(1.0f, 1.0f, -1.0f)),

    Triangle(glm::vec3(0, 0, -1.0f),
             glm::vec3(-1.0f, -1.0f, -1.0f),
             glm::vec3(1.0f, -1.0f, -1.0f),
             glm::vec3(1.0f, 1.0f, -1.0f)),

    Triangle(glm::vec3(1.0f, 0, 0),
             glm::vec3(1.0f, 1.0f, -1.0f),
             glm::vec3(1.0f, -1.0f, -1.0f),
             glm::vec3(1.0f, -1.0f, 1.0f)),

    Triangle(glm::vec3(1.0f, 0, 0),
             glm::vec3(1.0f, 1.0f, -1.0f),
             glm::vec3(1.0f, 1.0f, 1.0f),
             glm::vec3(1.0f, -1.0f, 1.0f)),

    Triangle(glm::vec3(0, -1.0f, 0),
             glm::vec3(1.0f, -1.0f, 1.0f),
             glm::vec3(1.0f, -1.0f, -1.0f),
             glm::vec3(-1.0f, -1.0f, -1.0f)),

    Triangle(glm::vec3(0, -1.0f, 0),
             glm::vec3(1.0f, -1.0f, 1.0f),
             glm::vec3(-1.0f, -1.0f, 1.0f),
             glm::vec3(-1.0f, -1.0f, -1.0f)),

    Triangle(glm::vec3(-1.0f, 0, 0),
             glm::vec3(-1.0f, -1.0f, -1.0f),
             glm::vec3(-1.0f, 1.0f, 1.0f),
             glm::vec3(-1.0f, 1.0f, -1.0f)),

    Triangle(glm::vec3(-1.0f, 0, 0),
             glm::vec3(-1.0f, -1.0f, -1.0f),
             glm::vec3(-1.0f, 1.0f, 1.0f),
             glm::vec3(-1.0f, -1.0f, 1.0f)),

    Triangle(glm::vec3(0, 0, 1.0f),
             glm::vec3(1.0f, -1.0f, 1.0f),
             glm::vec3(-1.0f, -1.0f, 1.0f),
             glm::vec3(-1.0f, 1.0f, 1.0f)),

    Triangle(glm::vec3(0, 0, 1.0f),
             glm::vec3(1.0f, -1.0f, 1.0f),
             glm::vec3(1.0f, 1.0f, 1.0f),
             glm::vec3(-1.0f, 1.0f, 1.0f)),

    Triangle(glm::vec3(0, 1.0f, 0),
             glm::vec3(1.0f, 1.0f, -1.0f),
             glm::vec3(-1.0f, 1.0f, -1.0f),
             glm::vec3(-1.0f, 1.0f, 1.0f)),

    Triangle(glm::vec3(0, 1.0f, 0),
             glm::vec3(1.0f, 1.0f, -1.0f),
             glm::vec3(1.0f, 1.0f, 1.0f),
             glm::vec3(-1.0f, 1.0f, 1.0f))
};
