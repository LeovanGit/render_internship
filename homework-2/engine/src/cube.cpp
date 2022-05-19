#include "cube.hpp"
#include "matrix.hpp"

Cube::Cube(glm::vec3 position,
           glm::vec3 angles,
           glm::vec3 scale,
           Material material) : 
    material(material)
{
    Transform t(position, angles, scale);
    model_matrix = t.toMat4();
    model_matrix_inv = glm::inverse(model_matrix);
}

bool Cube::intersect(Intersection & nearest, Ray & ray)
{
    bool found_intersection = false;

    // ray in MS
    glm::vec4 orig_ms = model_matrix_inv * glm::vec4(ray.origin, 1.0f);
    glm::vec4 dir_ms = model_matrix_inv * glm::vec4(ray.direction, 0.0f);

    Ray ray_ms(glm::vec3(orig_ms) / orig_ms.w,
               glm::normalize(glm::vec3(dir_ms)));

    // find nearest from all mesh in MS
    Intersection tmp;
    tmp.reset();
   
    for (int i = 0, size = mesh.size(); i != size; ++i)
    {
        found_intersection |= mesh[i].intersect(tmp, ray_ms);
    }
    if (!found_intersection) return false; // no intersection

    // back to WS
    glm::vec4 point_ws = model_matrix * glm::vec4(tmp.point, 1.0f);
    glm::vec3 point_ws_norm = glm::vec3(point_ws) / point_ws.w;

    float t = glm::length(point_ws_norm - ray.origin);

    if (t >= nearest.t) return false; // not nearest intersection

    glm::vec4 normal_ws = model_matrix * glm::vec4(tmp.normal, 0.0f);

    nearest.t = t;
    nearest.point = point_ws_norm;
    nearest.normal = glm::normalize(glm::vec3(normal_ws));

    nearest.material = material;
    nearest.type = TRIANGLE;

    return true;
}

// create one mesh for all cubes
std::vector <Triangle> Cube::mesh = {
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
