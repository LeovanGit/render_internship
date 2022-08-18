// #ifndef TRIANGLE_OCTREE_H
// #define TRIANGLE_OCTREE_H

// #include "glm.hpp"
// #include <limits>
// #include <vector>
// #include <array>
// #include <memory>
// #include <cassert>
// #include <algorithm>

// #include "box.hpp"

// namespace math
// {
// struct Ray;
// struct Mesh;

// struct MeshIntersection
// {
//     glm::vec3 pos;
//     glm::vec3 normal;
//     float near;
//     float t;
//     uint32_t triangle;

//     constexpr void reset(float near,
//                          float far = std::numeric_limits<float>::infinity())
//     {
//         this->near = near;
//         t = far;
//     }
    
//     bool valid() const { return std::isfinite(t); }
// };

// class TriangleOctree //: public NonCopyable
// {
// public:
//     const static int PREFFERED_TRIANGLE_COUNT;
//     const static float MAX_STRETCHING_RATIO;

//     void clear() { mesh = nullptr; }
//     bool inited() const { return mesh != nullptr; }

//     void initialize(const Mesh & mesh);

//     bool intersect(const Ray & ray,
//                    MeshIntersection & nearest) const;

// protected:
//     const Mesh * mesh = nullptr;
//     std::vector<uint32_t> triangles;

//     BoundingBox box;
//     BoundingBox initial_box;

//     std::unique_ptr<std::array<TriangleOctree, 8>> children;

//     void initialize(const Mesh & mesh,
//                     const BoundingBox & parent_box,
//                     const glm::vec3 & parent_center,
//                     int octet_index);

//     bool addTriangle(uint32_t triangle_index,
//                      const glm::vec3 & V1,
//                      const glm::vec3 & V2,
//                      const glm::vec3 & V3,
//                      const glm::vec3 & center);

//     bool intersectInternal(const Ray & ray,
//                            MeshIntersection & nearest) const;
// };
// } // namespace math
// #endif
