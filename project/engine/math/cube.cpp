#include "cube.hpp"

math::Cube::Cube(const glm::vec3 & position,
                 const EulerAngles & angles,
                 const glm::vec3 & scale)
{
    math::Transform t(position, angles, scale);
    model_matrix = t.toMat4();
    model_matrix_inv = glm::inverse(model_matrix);
}

void math::Cube::setPosition(const glm::vec3 & pos)
{
    model_matrix[3][0] = pos.x;
    model_matrix[3][1] = pos.y;
    model_matrix[3][2] = pos.z;

    model_matrix_inv = glm::inverse(model_matrix);
}

void math::Cube::addPosition(const glm::vec3 & offset)
{
    model_matrix[3][0] += offset.x;
    model_matrix[3][1] += offset.y;
    model_matrix[3][2] += offset.z;

    model_matrix_inv = glm::inverse(model_matrix);
}

bool math::Cube::intersect(math::Intersection & nearest,
                           const math::Ray & ray)
{
    bool found_intersection = false;

    // transform ray to Model Space
    glm::vec4 orig_h_ms = model_matrix_inv * glm::vec4(ray.origin, 1.0f);
    glm::vec4 dir_h_ms = model_matrix_inv * glm::vec4(ray.direction, 0.0f);

    Ray ray_ms(glm::vec3(orig_h_ms) / orig_h_ms.w,
               glm::vec3(dir_h_ms));

    // find nearest from all mesh in Model Space
    Intersection nearest_ms;
    nearest_ms.reset();
   
    for (int i = 0, size = mesh.size(); i != size; ++i)
    {
        found_intersection |= mesh[i].intersect(nearest_ms, ray_ms);
    }
    if (!found_intersection) return false; // no intersection

    // back to World Space
    glm::vec4 point_h_ws = model_matrix * glm::vec4(nearest_ms.point, 1.0f);
    glm::vec3 point_ws = glm::vec3(point_h_ws) / point_h_ws.w;

    float t = glm::length(point_ws - ray.origin) / glm::length(ray.direction);
    if (t >= nearest.t) return false; // not nearest intersection

    glm::vec4 normal_ws = model_matrix * glm::vec4(nearest_ms.normal, 0.0f);

    nearest.t = t;
    nearest.point = point_ws;
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
