#ifndef TRANSFORM_SYSTEM_H
#define TRANSFORM_SYSTEM_H

#include "spdlog.h"
#include "glm.hpp"

#include "solid_vector.hpp"
#include "matrices.hpp"

namespace engine
{
class TransformSystem
{
public:
    // deleted methods should be public for better error messages
    TransformSystem(const TransformSystem & other) = delete;
    void operator=(const TransformSystem & other) = delete;

    static void init();

    static TransformSystem * getInstance();

    static void del();
    
    math::SolidVector<math::Transform> transforms;
    
private:    
    TransformSystem() = default;
    ~TransformSystem() = default;
    
    static TransformSystem * instance;
};
} // namespace engine

#endif
