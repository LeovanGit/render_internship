#include "material.hpp"

Material::Material(const glm::vec3 & albedo,
                   float specular, 
                   float glossiness, 
                   const glm::vec3 & emission) :
                   albedo(albedo),
                   specular(specular),
                   glossiness(glossiness),
                   emission(emission) {}

