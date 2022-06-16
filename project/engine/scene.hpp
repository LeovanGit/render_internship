#ifndef SCENE_HPP
#define SCENE_HPP

#include <iostream>

#include <vector>
#include <cmath>
#include <memory>
#include "glm.hpp"
#include "gtc/constants.hpp"

#include "window.hpp"
#include "camera.hpp"
#include "sphere.hpp"
#include "ray.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "cube.hpp"
#include "material.hpp"
#include "intersection.hpp"
#include "euler_angles.hpp"
#include "hemisphere_ray_generation.hpp"
#include "constants.hpp"

#include "parallel_executor.hpp"

class Scene
{
protected:
    enum class IntersectedType
    {
        SPHERE,
        PLANE,
        CUBE,
        POINT_LIGHT,
        SPOT_LIGHT,
        EMPTY
    };

    class ObjRef
    {
    public:
        void * object;
        IntersectedType type;
    };

public:
    // ====================[SCENE OBJECTS]====================

    // composition of math::Sphere and Material
    // inheritance to avoid sphere.sphere
    class Sphere : public math::Sphere
    {
    public:
        Sphere(float radius,
               const glm::vec3 & origin,
               const Material & material) :
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
        Plane(const glm::vec3 & normal,
              const glm::vec3 & origin,
              const Material & material) :
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
        Cube(const glm::vec3 & position,
             const math::EulerAngles & angles,
             const glm::vec3 & scale,
             const Material & material) :
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
        DirectionalLight(const glm::vec3 & radiance,
                         const glm::vec3 & direction,
                         float solid_angle) :
                         radiance(radiance),
                         direction(direction),
                         solid_angle(solid_angle)
        {}
        
        // without visualisation because nothing depends on its position

        glm::vec3 radiance;
        glm::vec3 direction;
        float solid_angle;
    };

    class PointLight
    {
    public:
        PointLight(const glm::vec3 & position,
                   const glm::vec3 & radiance,
                   float radius) :
                   position(position),
                   radiance(radiance),
                   radius(radius)
        {}

        bool intersect(math::Intersection & nearest,
                       const math::Ray & ray,
                       ObjRef & obj_ref,
                       Material & material)
        {
            math::Sphere sphere(radius, position);

            if (sphere.intersect(nearest, ray))
            {               
                obj_ref.type = IntersectedType::POINT_LIGHT;
                obj_ref.object = this;
                material = Material(radiance,
                                    1.0f,
                                    0.0f,
                                    radiance,
                                    glm::vec3(0));

                return true;
            }
            return false;
        }

        float calculateSolidAngle(float L_length)
        {
            return (glm::pi<float>() * radius * radius) / (L_length * L_length);
        }

        glm::vec3 position;
        glm::vec3 radiance;
        float radius;
    };

    class SpotLight
    {
    public:
        SpotLight(const glm::vec3 & position,
                  const glm::vec3 & radiance,
                  float radius,
                  const glm::vec3 & direction,
                  float inner_angle,
                  float outer_angle) :
                  position(position),
                  radiance(radiance),
                  radius(radius),
                  direction(direction),
                  inner_angle(inner_angle),
                  outer_angle(outer_angle)
        {}

        bool intersect(math::Intersection & nearest,
                       const math::Ray & ray,
                       ObjRef & obj_ref,
                       Material & material)
        {
            math::Sphere sphere(radius, position);

            if (sphere.intersect(nearest, ray))
            {               
                obj_ref.type = IntersectedType::SPOT_LIGHT;
                obj_ref.object = this;
                material = Material(radiance,
                                    1.0f,
                                    0.0f,
                                    radiance,
                                    glm::vec3(0));

                return true;
            }
            return false;
        }

        float calculateSolidAngle(float L_length)
        {
            return (glm::pi<float>() * radius * radius) / (L_length * L_length);
        }

        bool isPointIlluminated(const glm::vec3 & point)
        {
            glm::vec3 dir_to_point = glm::normalize(point - position);
            float cosa = glm::dot(dir_to_point, glm::normalize(direction));
            float cosb = cosf(glm::radians(outer_angle / 2.0f));

            return cosa > cosb;
        }

        float calculateAngularAttenuation(const glm::vec3 & point)
        {
            glm::vec3 dir_to_point = glm::normalize(point - position);
            float cosa = glm::dot(dir_to_point, glm::normalize(direction));

            float outer_cos = cosf(glm::radians(outer_angle / 2.0f));
            float inner_cos = cosf(glm::radians(inner_angle / 2.0f));

            float intensity = (cosa - outer_cos) / (inner_cos - outer_cos);
            
            return glm::clamp(intensity, 0.0f, 1.0f);
        }

        glm::vec3 position;
        glm::vec3 radiance;
        float radius;

        glm::vec3 direction;
        // in degrees
        float inner_angle;
        float outer_angle;
    };

    // ====================[OBJECT DECORATORS]====================
    class IObjectMover
    {
    public:
        virtual void move(const glm::vec3 & offset) = 0;

        virtual ~IObjectMover() {}
    };

    class SphereMover : public IObjectMover
    {
    public:
        SphereMover(math::Sphere & sphere) :
            sphere(sphere)
        {}

        virtual void move(const glm::vec3 & offset) override
        {
            sphere.origin += offset;
        }

        math::Sphere & sphere;
    };

    class CubeMover : public IObjectMover
    {
    public:
        CubeMover(math::Cube & cube) :
            cube(cube)
        {}

        virtual void move(const glm::vec3 & offset) override
        {
            cube.addPosition(offset);
        }

        math::Cube & cube;
    };

    class PointLightMover : public IObjectMover
    {
    public:
        PointLightMover(PointLight & point_light) :
            point_light(point_light)
        {}

        virtual void move(const glm::vec3 & offset) override
        {
            point_light.position += offset;
        }

        PointLight & point_light;
    };

    class SpotLightMover : public IObjectMover
    {
    public:
        SpotLightMover(SpotLight & point_light) :
            point_light(point_light)
        {}

        virtual void move(const glm::vec3 & offset) override
        {
            point_light.position += offset;
        }

        SpotLight & point_light;
    };

    std::vector<Sphere> spheres;
    std::vector<Plane> planes;
    std::vector<Cube> cubes;

    std::vector<DirectionalLight> d_lights;
    std::vector<PointLight> p_lights;
    std::vector<SpotLight> s_lights;

    bool is_smooth_reflection;
    bool is_global_illumination;
    bool is_image_ready;

public:
    explicit Scene() = default;

    Scene(const std::vector<Sphere> & spheres,
          const std::vector<Plane> & planes,
          const std::vector<Cube> & cube,
          const std::vector<DirectionalLight> & d_lights,
          const std::vector<PointLight> & p_lights,
          const std::vector<SpotLight> & s_lights);

    // overloaded: for render only
    bool findIntersection(math::Intersection & nearest,
                          const math::Ray & ray,
                          Material & material,
                          IntersectedType & type,
                          bool include_lights = true);

    class IntersectionQuery
    {
    public:
        IntersectionQuery() = default;

        math::Intersection nearest;
        Material material;

        // passing by pointer allows to define if
        // smth additional needs to be returned
        // nullptr = no need
        std::unique_ptr<IObjectMover> * mover;
    };

    // overloaded: for getting more data from findIntersection
    // if it's need
    bool findIntersection(const math::Ray & ray,
                          IntersectionQuery & query);

    glm::vec3 approximateClosestSphereDir(bool& intersects,
                                          glm::vec3 reflectionDir,
                                          float sphereCos,
                                          glm::vec3 sphereRelPos,
                                          glm::vec3 sphereDir,
                                          float sphereDist,
                                          float sphereRadius);
    
    void clampDirToHorizon(glm::vec3 & dir,
                           float & NoD,
                           glm::vec3 normal,
                           float minNoD);

    bool isVisible(const math::Intersection & nearest,
                   const glm::vec3 & dir_to_light);

    float ggxSmith(const float roughness_sqr,
                   const float NL,
                   const float NV);

    float ggxTrowbridgeReitz(const float roughness_sqr,
                             const float NH);

    glm::vec3 ggxSchlick(const float cosTheta,
                         const glm::vec3 & F0);

    // Lambertian diffuse BRDF
    glm::vec3 LambertBRDF(const Material & material,
                          float NL);

    // GGX Cook-Torrance specular BRDF
    glm::vec3 CookTorranceBRDF(float roughness_sqr,
                               const glm::vec3 & fresnel,
                               float NL,
                               float NV,
                               float NH,
                               float HL,
                               float solid_angle);

    // overloaded: for samples
    glm::vec3 PBR(const glm::vec3 & N,
                  const glm::vec3 & L,
                  const glm::vec3 & V,
                  const Material & material,
                  const glm::vec3 & radiance);

    // overloaded: approximation for one ray
    glm::vec3 PBR(const math::Intersection & nearest,
                  const Material & material,
                  const Camera & camera,
                  const math::Ray & ray);

    glm::vec3 calculatePixelEnergy(const math::Intersection & nearest,
                                   const Material & material,
                                   const Camera & camera,
                                   const math::Ray & ray,
                                   float depth = 0);

    glm::vec3 toneMappingACES(const glm::vec3 & color) const;

    glm::vec3 gammaCorrection(const glm::vec3 & color) const;

    void render(Window & win, Camera & camera);

protected:
    void findIntersectionInternal(math::Intersection & nearest,
                                  const math::Ray & ray,
                                  ObjRef & obj_ref,
                                  Material & material,
                                  bool include_lights = true);

};

#endif
