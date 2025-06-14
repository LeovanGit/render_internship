#include "additional.hpp"

namespace
{
typedef engine::OpaqueInstances oi;
typedef engine::DissolutionInstances di;
typedef engine::DisappearInstances dpi;
} // namespace 

namespace engine
{
void moveDissolutionToOpaqueInstances()
{
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::TransformSystem * trans_system = engine::TransformSystem::getInstance();
    float engine_time = engine::TimeSystem::getTimePoint();

    auto & per_model = mesh_system->dissolution_instances.per_model;
    for (uint32_t model = 0; model != per_model.size(); ++model)
    {
        bool should_move = false;
        std::vector<oi::Material> materials;
        uint32_t transform_id;

        auto & per_mesh = per_model[model].per_mesh;
        for (uint32_t mesh = 0; mesh != per_mesh.size(); ++mesh)
        {
            auto & per_material = per_mesh[mesh].per_material;
            for (uint32_t material = 0; material != per_material.size(); ++material)
            {
                auto & instances = per_material[material].instances;
                for (uint32_t i = 0; i != instances.size(); ++i)
                {
                    if (engine_time - instances[i].spawn_time > instances[i].animation_time)
                    {
                        should_move = true;

                        // Dissolve Material -> Opaque Material
                        auto & mat = per_material[material].material;
                        materials.push_back(oi::Material(mat.albedo,
                                                         mat.roughness,
                                                         mat.metalness,
                                                         mat.normal,
                                                         mat.is_directx_style_normal_map,
                                                         mat.is_double_sided,
                                                         mat.albedo_default,
                                                         mat.roughness_default,
                                                         mat.metalness_default));

                        transform_id = instances[i].transform_id;

                        instances.erase(instances.begin() + i--);
                    }
                }
                if (instances.size() == 0)
                    per_material.erase(per_material.begin() + material--);
            }
            if (per_material.size() == 0)
                per_mesh.erase(per_mesh.begin() + mesh--);
        }

        if (should_move)
        {
            oi::Instance instance(transform_id,
                                  mesh_system->getModelID(),
                                  per_model[model].model->getBox());
            
            mesh_system->addInstance<engine::OpaqueInstances>(per_model[model].model,
                                                              materials,
                                                              instance);
        }
        
        if (per_mesh.size() == 0)
            per_model.erase(per_model.begin() + model--);
    }
    
    mesh_system->dissolution_instances.updateInstanceBuffers();
    mesh_system->opaque_instances.updateInstanceBuffers();
}

void moveOpaqueToDisappearInstances(uint32_t model_id,
                                    float model_box_diameter,
                                    const glm::vec3 & sphere_origin)
{
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::TransformSystem * trans_system = engine::TransformSystem::getInstance();

    auto & per_model = mesh_system->opaque_instances.per_model;
    for (uint32_t model = 0; model != per_model.size(); ++model)
    {
        bool should_move = false;
        std::vector<dpi::Material> materials;
        uint32_t transform_id;

        auto & per_mesh = per_model[model].per_mesh;
        for (uint32_t mesh = 0; mesh != per_mesh.size(); ++mesh)
        {
            auto & per_material = per_mesh[mesh].per_material;
            for (uint32_t material = 0; material != per_material.size(); ++material)
            {
                auto & instances = per_material[material].instances;
                for (uint32_t i = 0; i != instances.size(); ++i)
                {
                    if (instances[i].model_id == model_id)
                    {
                        should_move = true;

                        // Opaque Material -> Disappear Material
                        auto & mat = per_material[material].material;
                        materials.push_back(dpi::Material(mat.albedo,
                                                          mat.roughness,
                                                          mat.metalness,
                                                          mat.normal,
                                                          mat.is_directx_style_normal_map,
                                                          mat.is_double_sided,
                                                          mat.albedo_default,
                                                          mat.roughness_default,
                                                          mat.metalness_default));

                        transform_id = instances[i].transform_id;

                        instances.erase(instances.begin() + i--);
                    }
                }
                if (instances.size() == 0)
                    per_material.erase(per_material.begin() + material--);
            }
            if (per_material.size() == 0)
                per_mesh.erase(per_mesh.begin() + mesh--);
        }

        if (should_move)
        {
            dpi::Instance instance(transform_id,
                                   mesh_system->getModelID(),
                                   model_box_diameter,
                                   TimeSystem::getTimePoint(),
                                   5.0f,
                                   sphere_origin);
            
            mesh_system->addInstance<engine::DisappearInstances>(per_model[model].model,
                                                                 materials,
                                                                 instance);
        }
        
        if (per_mesh.size() == 0)
            per_model.erase(per_model.begin() + model--);
    }
    
    mesh_system->disappear_instances.updateInstanceBuffers();
    mesh_system->opaque_instances.updateInstanceBuffers(); 
}

void updateDisappearInstances()
{
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    float engine_time = engine::TimeSystem::getTimePoint();

    auto & per_model = mesh_system->disappear_instances.per_model;
    for (uint32_t model = 0; model != per_model.size(); ++model)
    {
        auto & per_mesh = per_model[model].per_mesh;
        for (uint32_t mesh = 0; mesh != per_mesh.size(); ++mesh)
        {
            auto & per_material = per_mesh[mesh].per_material;
            for (uint32_t material = 0; material != per_material.size(); ++material)
            {
                auto & instances = per_material[material].instances;
                for (uint32_t i = 0; i != instances.size(); ++i)
                {
                    if (engine_time - instances[i].spawn_time > instances[i].animation_duration)
                    {                        
                        instances.erase(instances.begin() + i--);
                    }
                }
                if (instances.size() == 0)
                    per_material.erase(per_material.begin() + material--);
            }
            if (per_material.size() == 0)
                per_mesh.erase(per_mesh.begin() + mesh--);
        }        
        if (per_mesh.size() == 0)
            per_model.erase(per_model.begin() + model--);
    }
    
    mesh_system->disappear_instances.updateInstanceBuffers();
}
} // namespace engine
