#include "scene.hpp"
#include "constants.hpp"
#include "intersection.hpp"

namespace
{
constexpr bool SHADOWS = false;

constexpr float ROUGHNESS_THRESHOLD = 0.1f;
constexpr int RECURSION_DEPTH = 10;
constexpr int SAMPLES_COUNT = 500;

// ambient fog-rainy sky
constexpr glm::vec3 COLOR_SKY(0.353f, 0.5f, 0.533f);
constexpr float AMBIENT = 0.05f;

const uint32_t numThreads = 
    std::fmax(1,
              std::fmax(int(ParallelExecutor::MAX_THREADS) - 4,
                        int(ParallelExecutor::HALF_THREADS)));
} // namespace

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
             s_lights(s_lights),
             is_smooth_reflection(false),
             is_global_illumination(false),
             is_image_ready(false)
{}

bool Scene::findIntersection(math::Intersection & nearest,
                             const math::Ray & ray,
                             Material & material,
                             IntersectedType & type,
                             bool include_lights)
{
    ObjRef object = { nullptr, IntersectedType::EMPTY };

    findIntersectionInternal(nearest, ray, object, material, include_lights);

    type = object.type;
    return object.type != IntersectedType::EMPTY;
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
                                     Material & material,
                                     bool include_lights)
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

    // disable light sources reflection in mirrors
    if (!include_lights) return;

    for (int i = 0, size = p_lights.size(); i != size; ++i)
    {
        p_lights[i].intersect(nearest, ray, obj_ref, material);
    }

    for (int i = 0, size = s_lights.size(); i != size; ++i)
    {
        s_lights[i].intersect(nearest, ray, obj_ref, material);
    }
}

// May return direction pointing beneath surface horizon (dot(N, dir) < 0),
// use clampDirToHorizon to fix it.
// sphereCos is cosine of light sphere solid angle.
// sphereRelPos is position of a sphere relative to surface:
// sphereDir == normalize(sphereRelPos)
// sphereDir * sphereDist == sphereRelPos
glm::vec3 Scene::approximateClosestSphereDir(bool & intersects,
                                             glm::vec3 reflectionDir,
                                             float sphereCos,
                                             glm::vec3 sphereRelPos,
                                             glm::vec3 sphereDir,
                                             float sphereDist,
                                             float sphereRadius)
{
    float RoS = dot(reflectionDir, sphereDir);

    intersects = RoS >= sphereCos;
    if (intersects) return reflectionDir;
    if (RoS < 0.0) return sphereDir;

    glm::vec3 closestPointDir = 
        normalize(reflectionDir * sphereDist * RoS - sphereRelPos);

    return normalize(sphereRelPos + sphereRadius * closestPointDir);
}

// Input dir and NoD is N and NoL in a case of lighting computation 
void Scene::clampDirToHorizon(glm::vec3 & dir,
                              float & NoD,
                              glm::vec3 normal,
                              float minNoD)
{
    if (NoD < minNoD)
    {
        dir = normalize(dir + (minNoD - NoD) * normal);
        NoD = minNoD;
    }
}

bool Scene::isVisible(const math::Intersection & nearest,
                      const glm::vec3 & dir_to_light)
{
    if (!SHADOWS) return true;

    math::Ray ray;
    ray.direction = dir_to_light;
    ray.origin = nearest.point + math::EPSILON * nearest.normal;

    math::Intersection tmp;
    tmp.reset();

    Material material;
    IntersectedType type;

    bool found_intersection = findIntersection(tmp, ray, material, type);

    return (found_intersection && (type == IntersectedType::POINT_LIGHT ||
                                   type == IntersectedType::SPOT_LIGHT)) ||
           !found_intersection; // for directional
}

glm::vec3 Scene::calculatePointLights(const math::Intersection & nearest,
                                      const Camera & camera,
                                      const Material & material)
{
    glm::vec3 color(0);

    float roughness_sqr = material.roughness * material.roughness;

    glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
    glm::vec3 V_reflected = glm::reflect(-V, nearest.normal);
    float NV = glm::clamp(glm::dot(nearest.normal, V), 0.0f, 1.0f);

    for (int i = 0, size = p_lights.size(); i != size; ++i)
    {
        glm::vec3 L = p_lights[i].position - nearest.point;
        float L_length = glm::length(L);
        glm::vec3 L_norm = glm::normalize(L);

        // shadows
        if (!isVisible(nearest, L_norm)) continue;

        // light as solid angle
        float solid_angle = p_lights[i].calculateSolidAngle(L_length);

        // angular diameter of solid angle:
        float sina = p_lights[i].radius / L_length;
        float cosa = sqrtf(1.0f - sina * sina);

        glm::vec3 H = glm::normalize(L_norm + V);
        float NL = glm::clamp(glm::dot(nearest.normal, L_norm), 0.0f, 1.0f);

        // L2 considers size of the light source (only for specular)
        bool intersects = false;
        glm::vec3 L2 = approximateClosestSphereDir(intersects,
                                                   V_reflected,
                                                   cosa,
                                                   L,
                                                   L_norm,
                                                   L_length,
                                                   p_lights[i].radius);

        clampDirToHorizon(L2, NL, nearest.normal, 0.0f);

        glm::vec3 H2 = glm::normalize(L2 + V);

        float NL2 = glm::clamp(glm::dot(nearest.normal, L2), 0.0f, 1.0f);
        float NH2 = glm::clamp(glm::dot(nearest.normal, H2), 0.0f, 1.0f);
        float HL2 = glm::clamp(glm::dot(H, L2), 0.0f, 1.0f);

        glm::vec3 specular = CookTorranceBRDF(roughness_sqr,
                                              material.fresnel,
                                              NL2,
                                              NV,
                                              NH2,
                                              HL2,
                                              solid_angle);

        glm::vec3 diffuse = LambertBRDF(material, NL);

        color += (diffuse * solid_angle * NL +
                  specular) * p_lights[i].radiance;
    }

    return color;
}

glm::vec3 Scene::calculateSpotLights(const math::Intersection & nearest,
                                     const Camera & camera,
                                     const Material & material)
{
    glm::vec3 color(0);

    float roughness_sqr = material.roughness * material.roughness;
    
    glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
    glm::vec3 V_reflected = glm::reflect(-V, nearest.normal);
    float NV = glm::clamp(glm::dot(nearest.normal, V), 0.0f, 1.0f);

    for (int i = 0, size = s_lights.size(); i != size; ++i)
    {
        glm::vec3 L = s_lights[i].position - nearest.point;
        float L_length = glm::length(L);
        glm::vec3 L_norm = glm::normalize(L);

        // shadows
        if (!isVisible(nearest, L)) continue;

        // if (!s_lights[i].isPointIlluminated(nearest.point)) continue;
        float angular_attenuation =
            s_lights[i].calculateAngularAttenuation(nearest.point);
        if (angular_attenuation <= 0) continue;

        // light as solid angle
        float solid_angle = s_lights[i].calculateSolidAngle(L_length);

        // angular diameter of solid angle:
        float sina = s_lights[i].radius / L_length;
        float cosa = sqrtf(1.0f - sina * sina);

        glm::vec3 H = glm::normalize(L_norm + V);
        float NL = glm::clamp(glm::dot(nearest.normal, L_norm), 0.0f, 1.0f);

        // L2 considers size of the light source (only for specular)
        bool intersects = false;
        glm::vec3 L2 = approximateClosestSphereDir(intersects,
                                                   V_reflected,
                                                   cosa,
                                                   L,
                                                   L_norm,
                                                   L_length,
                                                   s_lights[i].radius);

        clampDirToHorizon(L2, NL, nearest.normal, 0.0f);

        glm::vec3 H2 = glm::normalize(L2 + V);

        float NL2 = glm::clamp(glm::dot(nearest.normal, L2), 0.0f, 1.0f);
        float NH2 = glm::clamp(glm::dot(nearest.normal, H2), 0.0f, 1.0f);
        float HL2 = glm::clamp(glm::dot(H, L2), 0.0f, 1.0f);

        glm::vec3 specular = CookTorranceBRDF(roughness_sqr,
                                              material.fresnel,
                                              NL2,
                                              NV,
                                              NH2,
                                              HL2,
                                              solid_angle);

        glm::vec3 diffuse = LambertBRDF(material, NL);

        // soft spotlight
        specular *= angular_attenuation;
        diffuse *= angular_attenuation;

        color += (diffuse * solid_angle * NL +
                  specular) * s_lights[i].radiance;
    }

    return color;
}

glm::vec3 Scene::calculateDirectionalLights(const math::Intersection & nearest,
                                            const Camera & camera,
                                            const Material & material)
{
    glm::vec3 color(0);

    float roughness_sqr = material.roughness * material.roughness;
    
    glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
    float NV = glm::clamp(glm::dot(nearest.normal, V), 0.0f, 1.0f);

    for (int i = 0, size = d_lights.size(); i != size; ++i)
    {
        glm::vec3 L = -glm::normalize(d_lights[i].direction);

        // shadows
        if (!isVisible(nearest, L)) continue;

        glm::vec3 H = glm::normalize(L + V);

        float NL = glm::clamp(glm::dot(nearest.normal, L), 0.0f, 1.0f);
        float NH = glm::clamp(glm::dot(nearest.normal, H), 0.0f, 1.0f);
        float HL = glm::clamp(glm::dot(H, L), 0.0f, 1.0f);

        glm::vec3 specular = CookTorranceBRDF(roughness_sqr,
                                              material.fresnel,
                                              NL,
                                              NV,
                                              NH,
                                              HL,
                                              d_lights[i].solid_angle);

        glm::vec3 diffuse = LambertBRDF(material, NL);
        
        color += (diffuse * d_lights[i].solid_angle * NL +
                  specular) * d_lights[i].radiance;
    }

    return color;
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

    float D = roughness_sqr / (math::PI  * denom * denom);

    return D;
}

glm::vec3 Scene::ggxSchlick(const float cosTheta,
                            const glm::vec3 & F0)
{
    return F0 + (1.0f - F0) * powf(1.0f - cosTheta, 5.0f);
}

glm::vec3 Scene::LambertBRDF(const Material & material,
                             float NL)
{
    glm::vec3 F = ggxSchlick(NL, material.fresnel);

    return material.albedo *
           (1.0f - material.metalness) *
           (1.0f - F) * NL / math::PI;
}

glm::vec3 Scene::CookTorranceBRDF(float roughness_sqr,
                                  const glm::vec3 & fresnel,
                                  float NL,
                                  float NV,
                                  float NH,
                                  float HL,
                                  float solid_angle)
{
    float G = ggxSmith(roughness_sqr, NL, NV);
    float D = ggxTrowbridgeReitz(roughness_sqr, NH);
    glm::vec3 F = ggxSchlick(HL, fresnel);

    // 1. clamp NDF to avoid light reflection being 
    // brighter than a light source
    // 2. NL was reduced with NL in integral!!!
    float D_norm = fmin(1.0f,
                        solid_angle * D /
                        (4 * NV));

    return D_norm * F * G;
}

// L - ray from intersection point to light source
// V - ray from intersection point to camera
// H - vector between L and V
glm::vec3 Scene::PBR(const glm::vec3 & N,
                     const glm::vec3 & L,
                     const glm::vec3 & V,
                     const Material & material,
                     const glm::vec3 & radiance)
{
    float roughness_sqr = material.roughness * material.roughness;

    glm::vec3 H = glm::normalize(L + V);

    float NL = glm::clamp(glm::dot(N, L), 0.0f, 1.0f);
    float NV = glm::clamp(glm::dot(N, V), 0.0f, 1.0f);
    float NH = glm::clamp(glm::dot(N, H), 0.0f, 1.0f);
    float HL = glm::clamp(glm::dot(H, L), 0.0f, 1.0f);

    glm::vec3 specular = CookTorranceBRDF(roughness_sqr,
                                          material.fresnel,
                                          NL,
                                          NV,
                                          NH,
                                          HL,
                                          1.0f);

    glm::vec3 diffuse = LambertBRDF(material, NL);

    return (diffuse * NL + specular) * radiance;
}

// L - ray from intersection point to light source
// V - ray from intersection point to camera
// H - vector between L and V
glm::vec3 Scene::PBR(const math::Intersection & nearest,
                     const Material & material,
                     const Camera & camera,
                     const math::Ray & ray)
{        
    glm::vec3 color(0);

    // POINT LIGHTS
    color += calculatePointLights(nearest, camera, material);

    // SPOTLIGHTS
    color += calculateSpotLights(nearest, camera, material);

    // DIRECTIONAL LIGHTS
    color += calculateDirectionalLights(nearest, camera, material);

    return color;
}

glm::vec3 Scene::calculatePixelEnergy(const math::Intersection & nearest,
                                      const Material & material,
                                      const Camera & camera,
                                      const math::Ray & ray,
                                      float depth)
{
    glm::vec3 color = material.emission +
                      PBR(nearest, material, camera, ray);

    // AMBIENT
    if (is_global_illumination)
    {
        glm::vec3 ambient(0);

        for (int i = 0; i != SAMPLES_COUNT; ++i)
        {
            math::Basis basis;
            basis.x = nearest.normal;
            math::onb_frisvad(basis.x, basis.y, basis.z);

            glm::mat3 transform(basis.y,
                                basis.x,
                                basis.z);

            glm::vec3 rand_point = math::generate_point_on_hemisphere(i, SAMPLES_COUNT);
            
            math::Ray ray_GI;
            ray_GI.origin = nearest.point + math::EPSILON * nearest.normal;
            // rotate hemisphere to our fragment normal
            ray_GI.direction = glm::normalize(transform * rand_point);
          
            math::Intersection nearest_GI;
            nearest_GI.reset();

            Material material_GI;

            IntersectedType type_GI;

            if (findIntersection(nearest_GI,
                                 ray_GI,
                                 material_GI,
                                 type_GI,
                                 false))
            {
                glm::vec3 L = nearest_GI.point - nearest.point;

                glm::vec3 radiance = material_GI.albedo;

                glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
                L = glm::normalize(L);

                ambient += PBR(nearest.normal,
                               L,
                               V,
                               material,
                               radiance);
            }        
        }

        color += ambient * 2.0f * math::PI / float(SAMPLES_COUNT);
    }
    else color += material.albedo * AMBIENT;

    // MIRRORS
    if (is_smooth_reflection && 
        material.roughness < ROUGHNESS_THRESHOLD &&
        depth < RECURSION_DEPTH)
    {
        // roughness[0, 0.1] -> intensity[max, min]
        float intensity = 1.0f - 10.0f * material.roughness;

        math::Ray ray_reflected;
        ray_reflected.origin = nearest.point + math::EPSILON * nearest.normal;
        ray_reflected.direction = 
            glm::normalize(glm::reflect(ray.direction, nearest.normal));

        math::Intersection nearest_reflected;
        nearest_reflected.reset();

        Material material_reflected;
    
        IntersectedType type_reflected;

        // G = 1
        // D = 1
        glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
        glm::vec3 H = glm::normalize(V + ray_reflected.direction);
        float HL = glm::clamp(glm::dot(H, ray_reflected.direction), 0.0f, 1.0f);
        glm::vec3 F = ggxSchlick(HL, material.fresnel);

        if (findIntersection(nearest_reflected,
                             ray_reflected,
                             material_reflected,
                             type_reflected,
                             false))
        {
            color += calculatePixelEnergy(nearest_reflected,
                                          material_reflected,
                                          camera,
                                          ray_reflected,
                                          depth + 1) * F * intensity;
        }
        else
        {
            color += COLOR_SKY * F * intensity;
        }
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
    return glm::vec3(powf(color.x, 1.0f / math::GAMMA),
                     powf(color.y, 1.0f / math::GAMMA),
                     powf(color.z, 1.0f / math::GAMMA));
}

void Scene::render(Window & win, Camera & camera)
{
    if (is_global_illumination && is_image_ready) return;

    SIZE pixels_size = win.getPixelsSize();
    int width = pixels_size.cx;
    int height = pixels_size.cy;       

    // find CS corner points in WS
    glm::vec3 bottom_left_WS = camera.reproject(-1.0f, -1.0f);
    glm::vec3 bottom_right_WS = camera.reproject(1.0f, -1.0f);
    glm::vec3 top_left_WS = camera.reproject(-1.0f, 1.0f);

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

        glm::vec3 result_color = COLOR_SKY;

        if (findIntersection(nearest, ray, material, type))
        {
            if (type == IntersectedType::POINT_LIGHT ||
                type == IntersectedType::SPOT_LIGHT)
            {
                result_color = material.emission;
            }
            else
                result_color = calculatePixelEnergy(nearest, material, camera, ray);
        }

        // post-processing
        result_color = camera.adjustExposure(result_color);
        result_color = toneMappingACES(result_color);
        result_color = gammaCorrection(result_color);

        pixels[xy.y * width + xy.x] = RGB(result_color.z * 255,
                                          result_color.y * 255,
                                          result_color.x * 255);
    };
    
    executor.execute(func, width * height, 20);

    win.flush();

    if (is_global_illumination) is_image_ready = true;
}

