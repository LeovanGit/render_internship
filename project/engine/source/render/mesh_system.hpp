#ifndef MESH_SYSTEM_HPP
#define MESH_SYSTEM_HPP

#include "spdlog.h"
#include <memory>

#include "opaque_instances.hpp"
#include "model.hpp"
#include "matrices.hpp"

namespace engine
{
class MeshSystem
{
public:
    // deleted methods should be public for better error messages
    MeshSystem(const MeshSystem & other) = delete;
    void operator=(const MeshSystem & other) = delete;
    
    static void init();
    
    static MeshSystem * getInstance();

    static void del();
    
    void render();
    
    void addInstance(std::shared_ptr<Model> model,
                     std::vector<OpaqueInstances::Material> & materials,
                     const math::Transform & transform);

    OpaqueInstances opaque_instances;

private:
    MeshSystem() = default;
    ~MeshSystem() = default;
    
    static MeshSystem * instance;
};
} // namespace engine

#endif
