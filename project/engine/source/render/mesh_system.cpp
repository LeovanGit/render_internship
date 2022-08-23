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
                    glm::mat4 transform_inv = glm::inverse(trans_system->
                                                           transforms[instance.transform_id]);
                    glm::mat4 mesh_to_model_inv = glm::inverse(mesh_ranges[i].mesh_to_model);

                    // TriangleOctree stores vertices in mesh space
                    math::Ray ray_ms;
                    ray_ms.origin = mesh_to_model_inv *
                                    transform_inv *
                                    glm::vec4(ray_ws.origin, 1.0f);
                    ray_ms.direction = glm::normalize(mesh_to_model_inv *
                                                      transform_inv *
                                                      glm::vec4(ray_ws.direction, 0.0f));

                    if (octree[i].intersect(ray_ms, nearest))
                    {
                        nearest.transform_id = instance.transform_id;
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
                    glm::mat4 transform_inv = glm::inverse(trans_system->
                                                           transforms[instance.transform_id]);
                    glm::mat4 mesh_to_model_inv = glm::inverse(mesh_ranges[i].mesh_to_model);

                    // TriangleOctree stores vertices in mesh space
                    math::Ray ray_ms;
                    ray_ms.origin = mesh_to_model_inv *
                                    transform_inv *
                                    glm::vec4(ray_ws.origin, 1.0f);
                    ray_ms.direction = glm::normalize(mesh_to_model_inv *
                                                      transform_inv *
                                                      glm::vec4(ray_ws.direction, 0.0f));

                    if (octree[i].intersect(ray_ms, nearest))
                    {
                        nearest.transform_id = instance.transform_id;
                    }
                }
            }
        } 
    }
    
    return nearest.valid();
}
} // namespace engine
