#ifndef ADDITIONAL_HPP
#define ADDITIONAL_HPP

#include <vector>

#include "mesh_system.hpp"
#include "transform_system.hpp"
#include "time_system.hpp"
#include "opaque_instances.hpp"
#include "dissolution_instances.hpp"

namespace engine
{
void moveDissolutionToOpaqueInstances();
} // namespace engine

#endif
