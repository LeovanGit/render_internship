#include "material.hpp"

Material::Material(const glm::vec3 & albedo,
                   float glossiness,
                   float metalness,
                   const glm::vec3 & emission) :
                   albedo(albedo),
                   glossiness(glossiness),
                   metalness(metalness),
                   emission(emission) {}

