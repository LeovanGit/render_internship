#include "mesh_system.hpp"

namespace engine
{
MeshSystem * MeshSystem::instance = nullptr;

void MeshSystem::init()
{
    if (!instance) instance = new MeshSystem();
    else spdlog::error("MeshSystem::init() was called twice!");
}

MeshSystem * MeshSystem::getInstance()
{
    return instance;
}

void MeshSystem::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("MeshSystem::del() was called twice!");
}

void MeshSystem::render()
{
    opaque_instances.render();
    emissive_instances.render();
}

bool MeshSystem::findIntersection(const math::Ray & ray_ws,
                                  math::MeshIntersection & nearest)
{
    TransformSystem * trans_system = TransformSystem::getInstance();

    glm::vec3 pos_ws;
    
    for (auto & model: opaque_instances.per_model)
    {
        auto & octree = model.model->getOctree();
        auto & mesh_ranges = model.model->getMeshRanges();

        for (uint32_t i = 0, size = model.per_mesh.size(); i != size; ++i)
        {
            auto & mesh = model.per_mesh[i];
            
            for (auto & material: mesh.per_material)
            {
                for (auto & instance: material.instances)
                {
                    glm::mat4 mesh_to_model = mesh_ranges[i].mesh_to_model;
                    glm::mat4 transform =
                        trans_system->transforms[instance.transform_id].toMat4();

                    glm::mat4 mesh_to_model_inv = glm::inverse(mesh_to_model);
                    glm::mat4 transform_inv = glm::inverse(transform);
                                        
                    // TriangleOctree stores vertices in mesh space
                    math::Ray ray_ms;
                    ray_ms.origin = mesh_to_model_inv *
                                    transform_inv *
                                    glm::vec4(ray_ws.origin, 1.0f);
                    ray_ms.direction = mesh_to_model_inv *
                                       transform_inv *
                                       glm::vec4(ray_ws.direction, 0.0f);

                    if (octree[i].intersect(ray_ms, nearest))
                    {
                        nearest.transform_id = instance.transform_id;

                        pos_ws = transform *
                                 mesh_to_model *
                                 glm::vec4(nearest.pos, 1.0f);
                    }
                }
            }
        } 
    }

    for (auto & model: emissive_instances.per_model)
    {
        auto & octree = model.model->getOctree();
        auto & mesh_ranges = model.model->getMeshRanges();

        for (uint32_t i = 0, size = model.per_mesh.size(); i != size; ++i)
        {
            auto & mesh = model.per_mesh[i];
            
            for (auto & material: mesh.per_material)
            {
                for (auto & instance: material.instances)
                {
                    glm::mat4 mesh_to_model = mesh_ranges[i].mesh_to_model;
                    glm::mat4 transform =
                        trans_system->transforms[instance.transform_id].toMat4();

                    glm::mat4 mesh_to_model_inv = glm::inverse(mesh_to_model);
                    glm::mat4 transform_inv = glm::inverse(transform);

                    // TriangleOctree stores vertices in mesh space
                    math::Ray ray_ms;
                    ray_ms.origin = mesh_to_model_inv *
                                    transform_inv *
                                    glm::vec4(ray_ws.origin, 1.0f);
                    ray_ms.direction = mesh_to_model_inv *
                                       transform_inv *
                                       glm::vec4(ray_ws.direction, 0.0f);

                    if (octree[i].intersect(ray_ms, nearest))
                    {
                        nearest.transform_id = instance.transform_id;

                        pos_ws = transform *
                                 mesh_to_model *
                                 glm::vec4(nearest.pos, 1.0f);
                    }
                }
            }
        } 
    }

    if (!nearest.valid()) return false;

    nearest.t = glm::length(pos_ws - ray_ws.origin) / glm::length(ray_ws.direction);
    nearest.pos = pos_ws;

    return true;
}
} // namespace engine