#ifndef OBJECT_MOVER_HPP
#define OBJECT_MOVER_HPP

#include "glm.hpp"
#include "sphere.hpp"

class IObjectMover
{
public:
    virtual void move(const glm::vec3 & offset) = 0;
    virtual ~IObjectMover() {}
};

class SphereMover : public IObjectMover
{
public:
    SphereMover(Sphere & sphere);
    virtual void move(const glm::vec3 & offset) override;

    Sphere & sphere;
};

#endif
