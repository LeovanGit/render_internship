#include "scene.hpp"
#include "ray.hpp"

namespace
{
    constexpr float epsilon = 0.000001f;
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
             is_global_illumination(false)
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

glm::vec3 Scene::ggxSchlick(const float cosTheta,
                            const glm::vec3 & F0)
{
    return F0 + (1.0f - F0) * powf(1.0f - cosTheta, 5.0f);
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
    float roughness_sqr = (1.0f - material.glossiness) * 
        (1.0f - material.glossiness);

    glm::vec3 H = glm::normalize(L + V);

    float NL = glm::clamp(glm::dot(N, L), 0.0f, 1.0f);
    float NV = glm::clamp(glm::dot(N, V), 0.0f, 1.0f);
    float NH = glm::clamp(glm::dot(N, H), 0.0f, 1.0f);
    float HL = glm::clamp(glm::dot(H, L), 0.0f, 1.0f);

    // GGX Cook-Torrance specular BRDF
    float G = ggxSmith(roughness_sqr, NL, NV);
    float D = ggxTrowbridgeReitz(roughness_sqr, NH);
    glm::vec3 F = ggxSchlick(HL, material.fresnel);

    glm::vec3 specular = D * F * G / (4 * NV * NL + epsilon);

    // Lambertian diffuse BRDF
    glm::vec3 diffuse = material.albedo * (1.0f - material.metalness) *
                        (1.0f - ggxSchlick(NL, material.fresnel)) / PI;

    return (diffuse + specular) * radiance * NL;
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

    float roughness_sqr = (1.0f - material.glossiness) * 
                          (1.0f - material.glossiness);

    glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
    glm::vec3 V_reflected = math::reflect(V, nearest.normal);
    float NV = glm::clamp(glm::dot(nearest.normal, V), 0.0f, 1.0f);

    // POINT LIGHTS
    for (int i = 0, size = p_lights.size(); i != size; ++i)
    {
        glm::vec3 L = p_lights[i].position - nearest.point;
        if (!isVisible(nearest, glm::normalize(L))) continue;

        // light as solid angle
        float L_length = glm::length(L);
        float solid_angle = (PI * p_lights[i].radius * p_lights[i].radius) /
                            (L_length * L_length);

        // flat angle of solid angle:
        // float angle = 2.0f * asinf(p_lights[i].radius / L_length);
        // formula which depends on solid angle:
        float angle = 2.0f * asinf(sqrtf(solid_angle / PI));

        // L2 considers size of the light source (use only for specular)
        bool intersects = false;
        glm::vec3 L2 = approximateClosestSphereDir(intersects,
                                                   V_reflected,
                                                   glm::cos(angle),
                                                   L,
                                                   glm::normalize(L),
                                                   L_length,
                                                   p_lights[i].radius);

        L = glm::normalize(L);
        glm::vec3 H = glm::normalize(L + V);

        float NL = glm::clamp(glm::dot(nearest.normal, L), 0.0f, 1.0f);
        // float NH = glm::clamp(glm::dot(nearest.normal, H), 0.0f, 1.0f);
        // float HL = glm::clamp(glm::dot(H, L), 0.0f, 1.0f);

        clampDirToHorizon(L2, NL, nearest.normal, 0.0f);
        glm::vec3 H2 = glm::normalize(L2 + V);

        float NL2 = glm::clamp(glm::dot(nearest.normal, L2), 0.0f, 1.0f);
        float NH2 = glm::clamp(glm::dot(nearest.normal, H2), 0.0f, 1.0f);
        float HL2 = glm::clamp(glm::dot(H, L2), 0.0f, 1.0f);

        // GGX Cook-Torrance specular BRDF
        float G = ggxSmith(roughness_sqr, NL2, NV);
        float D = ggxTrowbridgeReitz(roughness_sqr, NH2);
        glm::vec3 F = ggxSchlick(HL2, material.fresnel);

        // clamp NDF to avoid light reflection being brighter than
        // a light source
        // epsilon to avoid division by zero
        // float D_normalized = fmin(1.0f, solid_angle * D / (4 * NV * NL + epsilon));
        // (/ NL2) in specular BRDF and (* NL2) in integral was reduced
        float D_normalized = fmin(1.0f, solid_angle * D / (4 * NV + epsilon));
        glm::vec3 specular = F * G * D_normalized;

        // Lambertian diffuse BRDF
        glm::vec3 diffuse = material.albedo * (1.0f - material.metalness) *
                            (1.0f - ggxSchlick(NL, material.fresnel)) / PI;

        color += (diffuse * solid_angle * NL + specular) * p_lights[i].radiance;
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
        glm::vec3 F = ggxSchlick(HL, material.fresnel);

        // Lambertian diffuse BRDF
        glm::vec3 diffuse = material.albedo * (1.0f - material.metalness) *
                            (1.0f - ggxSchlick(NL, material.fresnel)) / PI;

        // clamp NDF to avoid light reflection being brighter than
        // a light source
        // epsilon to avoid division by zero
        float D_normalized = fmin(1.0f, d_lights[i].solid_angle * D /
                                  (4 * NV * NL + epsilon));
        glm::vec3 specular = F * G * D_normalized;

        color += (diffuse * d_lights[i].solid_angle + specular) *
                 d_lights[i].radiance * NL;
    }

    // SPOTLIGHTS
    for (int i = 0, size = s_lights.size(); i != size; ++i)
    {
        glm::vec3 L = s_lights[i].position - nearest.point;
        if (!isVisible(nearest, glm::normalize(L))) continue;

        // if (!s_lights[i].isPointIlluminated(nearest.point)) continue;
        float intensity = s_lights[i].calculateIntensity(nearest.point);
        if (intensity <= 0) continue;

        // light as solid angle
        float L_length = glm::length(L);
        float solid_angle = (PI * s_lights[i].radius * s_lights[i].radius) /
                            (L_length * L_length);

        // flat angle of solid angle:
        // float angle = 2.0f * asinf(p_lights[i].radius / L_length);
        // formula which depends on solid angle:
        float angle = 2.0f * asinf(sqrtf(solid_angle / PI));

        // L2 considers size of the light source (use only for specular)
        bool intersects = false;
        glm::vec3 L2 = approximateClosestSphereDir(intersects,
                                                   V_reflected,
                                                   glm::cos(angle),
                                                   L,
                                                   glm::normalize(L),
                                                   L_length,
                                                   s_lights[i].radius);

        L = glm::normalize(L);
        glm::vec3 H = glm::normalize(L + V);

        float NL = glm::clamp(glm::dot(nearest.normal, L), 0.0f, 1.0f);
        // float NH = glm::clamp(glm::dot(nearest.normal, H), 0.0f, 1.0f);
        // float HL = glm::clamp(glm::dot(H, L), 0.0f, 1.0f);

        clampDirToHorizon(L2, NL, nearest.normal, 0.0f);
        glm::vec3 H2 = glm::normalize(L2 + V);

        float NL2 = glm::clamp(glm::dot(nearest.normal, L2), 0.0f, 1.0f);
        float NH2 = glm::clamp(glm::dot(nearest.normal, H2), 0.0f, 1.0f);
        float HL2 = glm::clamp(glm::dot(H, L2), 0.0f, 1.0f);

        // GGX Cook-Torrance specular BRDF
        float G = ggxSmith(roughness_sqr, NL2, NV);
        float D = ggxTrowbridgeReitz(roughness_sqr, NH2);
        glm::vec3 F = ggxSchlick(HL2, material.fresnel);

        // clamp NDF to avoid light reflection being brighter than
        // a light source
        // epsilon to avoid division by zero
        // float D_normalized = fmin(1.0f, solid_angle * D / (4 * NV * NL + epsilon));
        // (/ NL2) in specular BRDF and (* NL2) in integral was reduced
        float D_normalized = fmin(1.0f, solid_angle * D / (4 * NV + epsilon));
        glm::vec3 specular = F * G * D_normalized;

        // Lambertian diffuse BRDF
        glm::vec3 diffuse = material.albedo * (1.0f - material.metalness) *
                            (1.0f - ggxSchlick(NL, material.fresnel)) / PI;

        // soft spotlight
        specular *= intensity;
        diffuse *= intensity;

        color += (diffuse * solid_angle * NL + specular) * s_lights[i].radiance;
    }

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
            ray_GI.origin = nearest.point + EPSILON * nearest.normal;
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

        color += ambient * 2.0f * PI / float(SAMPLES_COUNT);
    }
    else color += material.albedo * AMBIENT;

    // MIRRORS
    if (is_smooth_reflection && 
        material.glossiness > SMOOTHNESS_THRESHOLD &&
        depth < RECURSION_DEPTH)
    {
        // roughness[0, 0.1] -> intensity[max, min]
        float intensity = 10.0f * material.glossiness - 9.0f;

        math::Ray ray_reflected;
        ray_reflected.origin = nearest.point + EPSILON * nearest.normal;
        ray_reflected.direction = 
            glm::normalize(math::reflect(-ray.direction, nearest.normal));

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
}

