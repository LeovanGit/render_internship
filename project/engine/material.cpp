#include "material.hpp"

Material::Material(const glm::vec3 & albedo,
                   float specular, 
                   float glossiness,
                   float metalness,
                   const glm::vec3 & emission) :
                   albedo(albedo),
                   specular(specular),
                   glossiness(glossiness),
                   metalness(metalness),
                   emission(emission) {}

