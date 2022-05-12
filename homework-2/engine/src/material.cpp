#include "material.hpp"

Material::Material(glm::vec3 albedo,
                   float specular, 
                   float glossiness, 
                   glm::vec3 emission) :
                   albedo(albedo),
                   specular(specular),
                   glossiness(glossiness),
                   emission(emission) {}

