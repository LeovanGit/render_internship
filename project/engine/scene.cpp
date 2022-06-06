#include "scene.hpp"
#include "common.hpp"

Scene::Scene(const std::vector<Sphere> & spheres,
             const std::vector<Plane> & planes,
             const std::vector<Cube> & cubes,
             const std::vector<DirectionalLight> & d_lights,
             const std::vector<PointLight> & p_lights,
             const std::vector<SpotLight> & s_lights) :
             spheres(spheres),
             planes(planes),
             cubes(cubes),
             d_lights(d_lights),
             p_lights(p_lights),
             s_lights(s_lights)
{}

bool Scene::findIntersection(math::Intersection & nearest,
                             const math::Ray & ray,
                             Material & material,
                             IntersectedType & type)
{
    ObjRef obj_ref = { nullptr, IntersectedType::EMPTY };

    findIntersectionInternal(nearest, ray, obj_ref, material);
    type = obj_ref.type;

    return obj_ref.type != IntersectedType::EMPTY;
}

bool Scene::findIntersection(const math::Ray & ray,
                             IntersectionQuery & query)
{
    ObjRef obj_ref = { nullptr, IntersectedType::EMPTY };

    findIntersectionInternal(query.nearest, ray, obj_ref, query.material);

    switch(obj_ref.type)
    {
        case IntersectedType::SPHERE:
        {
            Sphere * sphere = static_cast<Sphere *>(obj_ref.object);
            if (query.mover) query.mover->reset(new SphereMover(*sphere));

            break;
        }
        case IntersectedType::PLANE:
        {
            break;
        }
        case IntersectedType::CUBE:
        {
            Cube * cube = static_cast<Cube *>(obj_ref.object);
            if (query.mover) query.mover->reset(new CubeMover(*cube));

            break;
        }
        case IntersectedType::POINT_LIGHT:
        {
            PointLight * p_light = static_cast<PointLight *>(obj_ref.object);
            if (query.mover) query.mover->reset(new PointLightMover(*p_light));

            break;
        }
        case IntersectedType::SPOT_LIGHT:
        {
            SpotLight * s_light = static_cast<SpotLight *>(obj_ref.object);
            if (query.mover) query.mover->reset(new SpotLightMover(*s_light));

            break;
        }
        case IntersectedType::EMPTY:
        {
            break;
        }
    }

    return obj_ref.type != IntersectedType::EMPTY;
}


// iterate over all objects and find the nearest intersection
void Scene::findIntersectionInternal(math::Intersection & nearest,
                                     const math::Ray & ray,
                                     ObjRef & obj_ref,
                                     Material & material)
{
    nearest.reset();

    for (int i = 0, size = planes.size(); i != size; ++i)
    {
         planes[i].intersect(nearest, ray, obj_ref, material);
    }
    
    for (int i = 0, size = spheres.size(); i != size; ++i)
    {
        spheres[i].intersect(nearest, ray, obj_ref, material);
    }

    for (int i = 0, size = cubes.size(); i != size; ++i)
    {
        cubes[i].intersect(nearest, ray, obj_ref, material);
    }

    for (int i = 0, size = p_lights.size(); i != size; ++i)
    {
        p_lights[i].intersect(nearest, ray, obj_ref, material);
    }

    for (int i = 0, size = s_lights.size(); i != size; ++i)
    {
        s_lights[i].intersect(nearest, ray, obj_ref, material);
    }
}

bool Scene::isVisible(const math::Intersection & nearest,
                      const glm::vec3 & dir_to_light)
{
    if (!SHADOWS) return true;

    math::Ray ray;
    ray.direction = dir_to_light;
    ray.origin = nearest.point + EPSILON * nearest.normal;

    math::Intersection tmp;
    tmp.reset();

    Material material;
    IntersectedType type;

    bool found_intersection = findIntersection(tmp, ray, material, type);

    return (found_intersection && (type == IntersectedType::POINT_LIGHT ||
                                   type == IntersectedType::SPOT_LIGHT)) ||
           !found_intersection; // for directional
}

float Scene::ggxSmith(const float roughness_sqr,
                      const float NL,
                      const float NV)
{
    float NL_sqr = NL * NL;
    float NV_sqr = NV * NV;

    // real Smith
    // float G1 = 2.0f / (1.0f + sqrtf(1.0f + roughness_sqr * 
    //                                 (1 - NL_sqr ) / NL_sqr));

    // float G2 = 2.0f / (1.0f + sqrtf(1.0f + roughness_sqr * 
    //                                 (1 - NV_sqr ) / NV_sqr));

    // return G1 * G2;

    // fast Smith from Filament
    return 2.0f / (sqrtf(1 + roughness_sqr * (1 - NV_sqr) / NV_sqr) +
                   sqrtf(1 + roughness_sqr * (1 - NL_sqr) / NL_sqr));

}

float Scene::ggxTrowbridgeReitz(const float roughness_sqr,
                                const float NH)
{
    float NH_sqr = NH * NH;
    
    float denom = NH_sqr * (roughness_sqr - 1.0f) + 1.0f;

    float D = roughness_sqr / (PI * denom * denom);

    return D;
}

glm::vec3 Scene::fresnelSchlick(const float cosTheta,
                                const glm::vec3 & F0)
{
    return F0 + (1.0f - F0) * powf(1.0f - cosTheta, 5.0f);
}

// L - ray from intersection point to light source
// V - ray from intersection point to camera
// H - vector between L and V
// D - distance to light source
glm::vec3 Scene::PBR(const math::Intersection & nearest,
                     const Material & material,
                     const Camera & camera)
{
    glm::vec3 color = material.emission + material.albedo * AMBIENT;

    float roughness_sqr = (1.0f - material.glossiness) * 
                          (1.0f - material.glossiness);

    glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
    float NV = glm::clamp(glm::dot(nearest.normal, V), 0.0f, 1.0f);

    // POINT LIGHTS
    for (int i = 0, size = p_lights.size(); i != size; ++i)
    {
        glm::vec3 L = glm::normalize(p_lights[i].position - nearest.point);
        if (!isVisible(nearest, L)) continue;

        glm::vec3 H = glm::normalize(L + V);

        float NL = glm::clamp(glm::dot(nearest.normal, L), 0.0f, 1.0f);
        float NH = glm::clamp(glm::dot(nearest.normal, H), 0.0f, 1.0f);
        float HL = glm::clamp(glm::dot(H, L), 0.0f, 1.0f);

        // GGX Cook-Torrance specular BRDF
        float G = ggxSmith(roughness_sqr, NL, NV);
        float D = ggxTrowbridgeReitz(roughness_sqr, NH);
        glm::vec3 F0 = glm::mix(INSULATOR_F0,
                                material.albedo,
                                material.metalness);
        glm::vec3 F = fresnelSchlick(HL, F0);

        // epsilon to avoid division by zero
        glm::vec3 specular = 0.25f * D * F * G / (NV * NL + EPSILON);

        // Lambertian diffuse BRDF
        // albedo * (1 - metalness), because metals haven't diffuse light
        glm::vec3 diffuse = material.albedo * (1.0f - material.metalness) *
                            (1.0f - fresnelSchlick(NL, F0)) / PI;
        
        // light as solid angle
        float light_radius_sqr = p_lights[i].radius * p_lights[i].radius;
        float L_length = glm::length(p_lights[i].position - nearest.point);
        float w = PI * light_radius_sqr /
                  (L_length * L_length - light_radius_sqr);

        glm::vec3 radiance = p_lights[i].color * p_lights[i].power * w;

        glm::vec3 BRDF = diffuse + specular;
        BRDF.x = fmin(1.0f, BRDF.x);
        BRDF.y = fmin(1.0f, BRDF.y);
        BRDF.z = fmin(1.0f, BRDF.z);

        color += BRDF * radiance * NL;
    }

    // DIRECTIONAL LIGHTS
    for (int i = 0, size = d_lights.size(); i != size; ++i)
    {
        glm::vec3 L = -glm::normalize(d_lights[i].direction);
        if (!isVisible(nearest, L)) continue;

        glm::vec3 H = glm::normalize(L + V);

        float NL = glm::clamp(glm::dot(nearest.normal, L), 0.0f, 1.0f);
        float NH = glm::clamp(glm::dot(nearest.normal, H), 0.0f, 1.0f);
        float HL = glm::clamp(glm::dot(H, L), 0.0f, 1.0f);

        // GGX Cook-Torrance specular BRDF
        float G = ggxSmith(roughness_sqr, NL, NV);
        float D = ggxTrowbridgeReitz(roughness_sqr, NH);
        glm::vec3 F0 = glm::mix(INSULATOR_F0,
                                material.albedo,
                                material.metalness);
        glm::vec3 F = fresnelSchlick(HL, F0);

        // epsilon to avoid division by zero
        glm::vec3 specular = 0.25f * D * F * G / (NV * NL + EPSILON);

        // Lambertian diffuse BRDF
        // albedo * (1 - metalness), because metals haven't diffuse light
        glm::vec3 diffuse = material.albedo * (1.0f - material.metalness) *
                            (1.0f - fresnelSchlick(NL, F0)) / PI;
        
        glm::vec3 radiance = d_lights[i].color * d_lights[i].power;

        glm::vec3 BRDF = diffuse + specular;
        BRDF.x = fmin(1.0f, BRDF.x);
        BRDF.y = fmin(1.0f, BRDF.y);
        BRDF.z = fmin(1.0f, BRDF.z);

        color += BRDF * radiance * NL;
    }

    // SPOTLIGHTS
    for (int i = 0, size = s_lights.size(); i != size; ++i)
    {
        glm::vec3 L = glm::normalize(s_lights[i].position - nearest.point);
        if (!isVisible(nearest, L)) continue;

        // check if nearest.point under the spotlight cone
        if (glm::dot(-L, glm::normalize(s_lights[i].direction)) <
            cosf(glm::radians(s_lights[i].angle / 2))) continue;

        glm::vec3 H = glm::normalize(L + V);

        float NL = glm::clamp(glm::dot(nearest.normal, L), 0.0f, 1.0f);
        float NH = glm::clamp(glm::dot(nearest.normal, H), 0.0f, 1.0f);
        float HL = glm::clamp(glm::dot(H, L), 0.0f, 1.0f);

        // GGX Cook-Torrance specular BRDF
        float G = ggxSmith(roughness_sqr, NL, NV);
        float D = ggxTrowbridgeReitz(roughness_sqr, NH);
        glm::vec3 F0 = glm::mix(INSULATOR_F0,
                                material.albedo,
                                material.metalness);
        glm::vec3 F = fresnelSchlick(HL, F0);

        // epsilon to avoid division by zero
        glm::vec3 specular = 0.25f * D * F * G / (NV * NL + EPSILON);

        // Lambertian diffuse BRDF
        // albedo * (1 - metalness), because metals haven't diffuse light
        glm::vec3 diffuse = material.albedo * (1.0f - material.metalness) *
                            (1.0f - fresnelSchlick(NL, F0)) / PI;

        // light as solid angle
        float light_radius_sqr = s_lights[i].radius * s_lights[i].radius;
        float L_length = glm::length(s_lights[i].position - nearest.point);
        float w = PI * light_radius_sqr /
                  (L_length * L_length - light_radius_sqr);

        glm::vec3 radiance = s_lights[i].color * s_lights[i].power * w;       

        glm::vec3 BRDF = diffuse + specular;
        BRDF.x = fmin(1.0f, BRDF.x);
        BRDF.y = fmin(1.0f, BRDF.y);
        BRDF.z = fmin(1.0f, BRDF.z);

        color += BRDF * radiance * NL;
    }

    return color;
}

glm::vec3 Scene::toneMappingACES(const glm::vec3 & hdr) const
{
    glm::mat3 m1(0.59719f, 0.07600f, 0.02840f,
                 0.35458f, 0.90834f, 0.13383f,
                 0.04823f, 0.01566f, 0.83777f);

    glm::mat3 m2(1.60475f, -0.10208, -0.00327f,
                 -0.53108f,  1.10813, -0.07276f,
                 -0.07367f, -0.00605,  1.07602f);

    glm::vec3 v = m1 * hdr;    
    glm::vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    glm::vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    glm::vec3 ldr = glm::clamp(m2 * (a / b), 0.0f, 1.0f);

    return ldr;
}

glm::vec3 Scene::gammaCorrection(const glm::vec3 & color) const
{
    return glm::vec3(powf(color.x, 1.0f / GAMMA),
                     powf(color.y, 1.0f / GAMMA),
                     powf(color.z, 1.0f / GAMMA));
}

void Scene::render(Window & win, Camera & camera)
{ 
    SIZE pixels_size = win.getPixelsSize();
    int width = pixels_size.cx;
    int height = pixels_size.cy;       

    // find CS corner points in WS
    glm::vec3 bottom_left_WS = camera.generateWorldPointFromCS(-1.0f, -1.0f);
    glm::vec3 bottom_right_WS = camera.generateWorldPointFromCS(1.0f, -1.0f);
    glm::vec3 top_left_WS = camera.generateWorldPointFromCS(-1.0f, 1.0f);

    glm::vec3 near_plane_right = bottom_right_WS - bottom_left_WS;
    glm::vec3 near_plane_top = top_left_WS - bottom_left_WS;

    std::vector<int> & pixels = win.getPixels();

    math::Intersection nearest;
    nearest.reset();

    math::Ray ray;
    ray.origin = camera.getPosition();

    Material material;
    
    IntersectedType type;

    ParallelExecutor executor(numThreads);

    auto func = [this,
                 width,
                 height,
                 bottom_left_WS,
                 near_plane_right,
                 near_plane_top,
                 &camera,
                 &nearest,
                 &ray,
                 &material,
                 &type,
                 &pixels]
        (uint32_t threadIndex,
         uint32_t taskIndex)
        {
            // current pixel coords
            glm::vec2 xy(taskIndex % width,
                         taskIndex / width);

            // normalized [0; 1]
            glm::vec2 xy_norm(float(xy.x + 0.5f) / width,
                              1.0f - float(xy.y + 0.5f) / height); // inversed

            ray.direction = (bottom_left_WS +
                             xy_norm.x * near_plane_right +
                             xy_norm.y * near_plane_top) - ray.origin;

            if (findIntersection(nearest, ray, material, type))
            {
                glm::vec3 result_color(0.0f, 0.0f, 0.0f);
                
                if (type != IntersectedType::POINT_LIGHT &&
                    type != IntersectedType::SPOT_LIGHT)
                {
                    result_color = PBR(nearest, material, camera);
                    result_color = camera.adjustExposure(result_color);
                    result_color = toneMappingACES(result_color);
                }
                else
                {
                    result_color = material.albedo;
                }

                result_color = gammaCorrection(result_color);

                pixels[xy.y * width + xy.x] = RGB(result_color.z * 255,
                                                  result_color.y * 255,
                                                  result_color.x * 255);
            }
            else
            {
                // ambient fog-rainy sky
                glm::vec3 sky_color(0.353f, 0.5f, 0.533f);
                sky_color = camera.adjustExposure(sky_color);
                sky_color = toneMappingACES(sky_color);
                sky_color = gammaCorrection(sky_color);

                pixels[xy.y * width + xy.x] = RGB(sky_color.z * 255,
                                                  sky_color.y * 255,
                                                  sky_color.x * 255);
            }
        };
    
    executor.execute(func, width * height, 20);

    win.flush();
}

