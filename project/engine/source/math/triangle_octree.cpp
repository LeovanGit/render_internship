#include "triangle_octree.hpp"

namespace math
{
const int TriangleOctree::PREFFERED_TRIANGLE_COUNT = 32;
const float TriangleOctree::MAX_STRETCHING_RATIO = 1.05f;

inline const glm::vec3 & getPos(const Mesh & mesh,
                                uint32_t triangle_index,
                                uint32_t vertex_index)
{
    uint32_t index = mesh.triangles.empty() ?
        triangle_index * 3 + vertex_index :
        mesh.triangles[triangle_index].indices[vertex_index];
    
    return mesh.vertices[index].position;
}

void TriangleOctree::initialize(std::shared_ptr<Mesh> mesh)
{
    triangles.clear();
    triangles.shrink_to_fit();

    this->mesh = mesh;
    children = nullptr;

    const glm::vec3 eps = { 1e-5f, 1e-5f, 1e-5f };
    box = initial_box = { mesh->box.min - eps, mesh->box.max + eps };

    for (uint32_t i = 0, size = mesh->triangles.size(); i != size; ++i)
    {
        const glm::vec3 & V1 = getPos(*mesh, i, 0);
        const glm::vec3 & V2 = getPos(*mesh, i, 1);
        const glm::vec3 & V3 = getPos(*mesh, i, 2);

        glm::vec3 P = (V1 + V2 + V3) / 3.0f;

        bool inserted = addTriangle(i, V1, V2, V3, P);
        assert(inserted);
    }
}

void TriangleOctree::initialize(std::shared_ptr<Mesh> mesh,
                                const BoundingBox & parent_box,
                                const glm::vec3 & parent_center,
                                int octet_index)
{
    this->mesh = mesh;
    children = nullptr;

    const float eps = 1e-5f;

    if (octet_index % 2 == 0)
    {
        initial_box.min[0] = parent_box.min[0];
        initial_box.max[0] = parent_center[0];
    }
    else
    {
        initial_box.min[0] = parent_center[0];
        initial_box.max[0] = parent_box.max[0];
    }
		
    if (octet_index % 4 < 2)
    {
        initial_box.min[1] = parent_box.min[1];
        initial_box.max[1] = parent_center[1];
    }
    else
    {
        initial_box.min[1] = parent_center[1];
        initial_box.max[1] = parent_box.max[1];
    }

    if (octet_index < 4)
    {
        initial_box.min[2] = parent_box.min[2];
        initial_box.max[2] = parent_center[2];
    }
    else
    {
        initial_box.min[2] = parent_center[2];
        initial_box.max[2] = parent_box.max[2];
    }

    box = initial_box;
    glm::vec3 elongation = (MAX_STRETCHING_RATIO - 1.f) * box.size();

    if (octet_index % 2 == 0) box.max[0] += elongation[0];
    else box.min[0] -= elongation[0];

    if (octet_index % 4 < 2) box.max[1] += elongation[1];
    else box.min[1] -= elongation[1];

    if (octet_index < 4) box.max[2] += elongation[2];
    else box.min[2] -= elongation[2];
}

bool TriangleOctree::addTriangle(uint32_t triangle_index,
                                 const glm::vec3 & V1,
                                 const glm::vec3 & V2,
                                 const glm::vec3 & V3,
                                 const glm::vec3 & center)
{
    if (!initial_box.contains(center) ||
        !box.contains(V1) ||
        !box.contains(V2) ||
        !box.contains(V3))
    {
        return false;
    }

    if (children == nullptr)
    {
        if (triangles.size() < PREFFERED_TRIANGLE_COUNT)
        {
            triangles.emplace_back(triangle_index);
            return true;
        }
        else
        {
            glm::vec3 C = (initial_box.min + initial_box.max) / 2.0f;

            children.reset(new std::array<TriangleOctree, 8>());
            for (int i = 0; i < 8; ++i)
            {
                (*children)[i].initialize(mesh, initial_box, C, i);
            }

            std::vector<uint32_t> new_triangles;

            for (uint32_t index : triangles)
            {
                const glm::vec3 & P1 = getPos(*mesh, index, 0);
                const glm::vec3 & P2 = getPos(*mesh, index, 1);
                const glm::vec3 & P3 = getPos(*mesh, index, 2);

                glm::vec3 P = (P1 + P2 + P3) / 3.0f;

                int i = 0;
                for (; i < 8; ++i)
                {
                    if ((*children)[i].addTriangle(index, P1, P2, P3, P))
                        break;
                }

                if (i == 8) new_triangles.emplace_back(index);
            }

            triangles = std::move(new_triangles);
        }
    }

    int i = 0;
    for (; i < 8; ++i)
    {
        if ((*children)[i].addTriangle(triangle_index, V1, V2, V3, center))
            break;
    }

    if (i == 8) triangles.emplace_back(triangle_index);

    return true;
}

bool TriangleOctree::intersect(const Ray & ray,
                               MeshIntersection & nearest) const
{
    float box_t = nearest.t;
    if (!ray.intersect(box_t, box)) return false;

    return intersectInternal(ray, nearest);
}

bool TriangleOctree::intersectInternal(const Ray & ray,
                                       MeshIntersection & nearest) const
{
    {
        float box_t = nearest.t;
        if (!ray.intersect(box_t, box)) return false;
    }

    bool found = false;

    for (uint32_t i = 0; i < triangles.size(); ++i)
    {
        const glm::vec3 & V1 = getPos(*mesh, triangles[i], 0);
        const glm::vec3 & V2 = getPos(*mesh, triangles[i], 1);
        const glm::vec3 & V3 = getPos(*mesh, triangles[i], 2);

        if (ray.intersect(nearest, V1, V2, V3))
        {
            nearest.triangle = i;
            found = true;
        }
    }

    if (!children) return found;

    struct OctantIntersection
    {
        int index;
        float t;
    };

    std::array<OctantIntersection, 8> box_intersections;

    for (int i = 0; i < 8; ++i)
    {
        if ((*children)[i].box.contains(ray.origin))
        {
            box_intersections[i].index = i;
            box_intersections[i].t = 0.0f;
        }
        else
        {
            float box_t = nearest.t;
            if (ray.intersect(box_t, (*children)[i].box))
            {
                box_intersections[i].index = i;
                box_intersections[i].t = box_t;
            }
            else
            {
                box_intersections[i].index = -1;
            }
        }
    }

    std::sort(box_intersections.begin(), box_intersections.end(),
              [](const OctantIntersection& A, const OctantIntersection& B) -> bool { return A.t < B.t; });

    for (int i = 0; i < 8; ++i)
    {
        if (box_intersections[i].index < 0 || box_intersections[i].t > nearest.t)
            continue;

        if ((*children)[box_intersections[i].index].intersectInternal(ray, nearest))
        {
            found = true;
        }
    }

    return found;
}
}
