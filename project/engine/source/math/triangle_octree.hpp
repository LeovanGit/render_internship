#ifndef TRIANGLE_OCTREE_H
#define TRIANGLE_OCTREE_H

#include "glm.hpp"
#include <limits>
#include <vector>
#include <array>
#include <memory>
#include <cassert>
#include <algorithm>

#include "box.hpp"
#include "ray.hpp"
#include "vertex.hpp"
#include "mesh_intersection.hpp"

namespace math
{
struct Mesh
{
    struct Triangle
    {
        std::array<uint32_t, 3> indices;
    };
    
    Mesh() = default;
    Mesh(const std::vector<engine::Vertex> & vertices,
         const std::vector<Triangle> & triangles,
         const BoundingBox & box) :
         vertices(vertices),
         triangles(triangles),
         box(box)
    {}

    std::vector<engine::Vertex> vertices;
    std::vector<Triangle> triangles;
    BoundingBox box;
};

class TriangleOctree //: public NonCopyable
{
public:    
    const static int PREFFERED_TRIANGLE_COUNT;
    const static float MAX_STRETCHING_RATIO;

    void clear() { mesh = nullptr; }
    bool inited() const { return mesh != nullptr; }

    void initialize(std::shared_ptr<Mesh> mesh);

    bool intersect(const Ray & ray,
                   MeshIntersection & nearest) const;

protected:
    // const Mesh * mesh = nullptr;
    std::shared_ptr<Mesh> mesh = nullptr;
    std::vector<uint32_t> triangles;

    BoundingBox box;
    BoundingBox initial_box;

    std::unique_ptr<std::array<TriangleOctree, 8>> children;

    void initialize(std::shared_ptr<Mesh> mesh,
                    const BoundingBox & parent_box,
                    const glm::vec3 & parent_center,
                    int octet_index);

    bool addTriangle(uint32_t triangle_index,
                     const glm::vec3 & V1,
                     const glm::vec3 & V2,
                     const glm::vec3 & V3,
                     const glm::vec3 & center);

    bool intersectInternal(const Ray & ray,
                           MeshIntersection & nearest) const;
};
} // namespace math
#endif
