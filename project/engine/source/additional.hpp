#ifndef ADDITIONAL_HPP
#define ADDITIONAL_HPP

#include <vector>

#include "mesh_system.hpp"
#include "transform_system.hpp"
#include "time_system.hpp"
#include "opaque_instances.hpp"
#include "dissolution_instances.hpp"
#include "disappear_instances.hpp"

#include "model_manager.hpp"

namespace engine
{
void moveDissolutionToOpaqueInstances();

void moveOpaqueToDisappearInstances(uint32_t model_id,
                                    float model_box_diameter,
                                    const glm::vec3 & sphere_origin);

void updateDisappearInstances();
} // namespace engine

#endif
