#include "material.hpp"

Material::Material(const glm::vec3 & albedo,
                   float roughness,
                   float metalness,
                   const glm::vec3 & emission,
                   const glm::vec3 & fresnel) :
                   albedo(albedo),
                   roughness(roughness),
                   metalness(metalness),
                   emission(emission),
                   fresnel(fresnel)
{}

