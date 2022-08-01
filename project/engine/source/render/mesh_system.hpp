#ifndef MESH_SYSTEM_HPP
#define MESH_SYSTEM_HPP

#include "opaque_instances.hpp"
#include "model.hpp"
#include "matrices.hpp"

namespace engine
{
class MeshSystem
{
public:		
    void render();
    
    void addModel(Model * model,
                  const OpaqueInstances::Material & material,
                  const math::Transform & transform);

    OpaqueInstances opaque_instances;
};
} // namespace engine

#endif
