#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <cmath>
#include "glm.hpp"

#include "window.hpp"
#include "camera.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "cube.hpp"
#include "material.hpp"
#include "intersection.hpp"

constexpr int HEX_BLACK = 0x000000;
constexpr float LIGHT_SIZE = 10.0f;
constexpr float AMBIENT = 0.1f;
constexpr float DELTA = 0.001f;

class Scene
{
protected:
    enum class IntersectedType
    {
        SPHERE,
        PLANE,
        CUBE,
        LIGHT,
        EMPTY
    };

    class ObjRef
    {
    public:
        void * object;
        IntersectedType type;
    };

public:
    // composition of math::Sphere and Material
    // inheritance to avoid sphere.sphere
    class Sphere : public math::Sphere
    {
    public:
        Sphere(float radius,
               glm::vec3 origin,
               Material material) :
               math::Sphere(radius, origin),
               material(material)
            {}

        bool intersect(math::Intersection & nearest,
                       const math::Ray & ray,
                       ObjRef & obj_ref,
                       Material & material)
        {
            if (math::Sphere::intersect(nearest, ray))
            {
                obj_ref.type = IntersectedType::SPHERE;
                obj_ref.object = this;
                material = this->material;

                return true;
            }
            return false;
        }

        Material material;
    };

    // composition of math::Plane and Material
    // inheritance to avoid plane.plane
    class Plane : public math::Plane
    {
    public:
        Plane(glm::vec3 normal,
              glm::vec3 origin,
              Material material) :
              math::Plane(normal, origin),
              material(material)
            {}

        bool intersect(math::Intersection & nearest,
                       const math::Ray & ray,
                       ObjRef & obj_ref,
                       Material & material)
        {
            if (math::Plane::intersect(nearest, ray))
            {
                obj_ref.type = IntersectedType::PLANE;
                obj_ref.object = this;
                material = this->material;

                return true;
            }
            return false;
        }

        Material material;
    };

    // composition of math::Cube and Material
    // inheritance to avoid cube.cube
    class Cube : public math::Cube
    {
    public:
        Cube(glm::vec3 position,
             glm::vec3 angles,
             glm::vec3 scale,
             Material material) :
             math::Cube(position, angles, scale),
             material(material)
            {}

        bool intersect(math::Intersection & nearest,
                       const math::Ray & ray,
                       ObjRef & obj_ref,
                       Material & material)
        {
            if (math::Cube::intersect(nearest, ray))
            {
                obj_ref.type = IntersectedType::CUBE;
                obj_ref.object = this;
                material = this->material;

                return true;
            }
            return false;
        }

        Material material;
    };

    class DirectionalLight
    {
    public:
        DirectionalLight(glm::vec3 direction,
                         glm::vec3 color) :
                         direction(direction),
                         color(color)
        {}
        
        // without visualisation because nothing depends on its position

        glm::vec3 direction;
        glm::vec3 color;
    };

    class PointLight
    {
    public:
        PointLight(glm::vec3 position,
                   float radius,
                   glm::vec3 color) :
                   position(position),
                   radius(radius),
                   color(color)
        {}

        bool intersect(math::Intersection & nearest,
                       const math::Ray & ray,
                       ObjRef & obj_ref,
                       Material & material)
        {
            math::Sphere sphere(LIGHT_SIZE, position);

            if (sphere.intersect(nearest, ray))
            {               
                obj_ref.type = IntersectedType::LIGHT;
                obj_ref.object = this;
                material = Material(color, 0, 0, glm::vec3(0));

                return true;
            }
            return false;
        }

        glm::vec3 position;
        float radius;
        glm::vec3 color;
    };

    class SpotLight
    {
    public:
        SpotLight(glm::vec3 position,
                  float radius,
                  float angle,
                  glm::vec3 direction,
                  glm::vec3 color) :
                  position(position),
                  radius(radius),
                  angle(angle),
                  direction(direction),
                  color(color)
        {}

        bool intersect(math::Intersection & nearest,
                       const math::Ray & ray,
                       ObjRef & obj_ref,
                       Material & material)
        {
            math::Sphere sphere(LIGHT_SIZE, position);

            if (sphere.intersect(nearest, ray))
            {               
                obj_ref.type = IntersectedType::LIGHT;
                obj_ref.object = this;
                material = Material(color, 0, 0, glm::vec3(0));

                return true;
            }
            return false;
        }

        glm::vec3 position;
        float radius;
        float angle;
        glm::vec3 direction;
        glm::vec3 color;
    };

    std::vector<Sphere> spheres;
    std::vector<Plane> planes;
    std::vector<Cube> cubes;

    std::vector<DirectionalLight> d_lights;
    std::vector<PointLight> p_lights;
    std::vector<SpotLight> s_lights;

public:
    explicit Scene() = default;

    Scene(std::vector<Sphere> spheres,
          std::vector<Plane> planes,
          std::vector<Cube> cube,
          std::vector<DirectionalLight> d_lights,
          std::vector<PointLight> p_lights,
          std::vector<SpotLight> s_lights);

    // overloaded for render only
    bool findIntersection(math::Intersection & nearest,
                          const math::Ray & ray,
                          Material & material,
                          IntersectedType & type);
 
    bool isVisible(const math::Intersection & nearest,
                   const glm::vec3 & dir_to_light);

    glm::vec3 blinnPhong(const math::Intersection & nearest,
                         const Material & material,
                         const Camera & camera);

    void render(Window & win, Camera & camera);

protected:
    void findIntersectionInternal(math::Intersection & nearest,
                                  const math::Ray & ray,
                                  ObjRef & obj_ref,
                                  Material & material);

};

#endif
