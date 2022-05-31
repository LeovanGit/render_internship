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
                             Material *& material,
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
                                     Material *& material)
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

    Material * material;
    IntersectedType type;

    bool found_intersection = findIntersection(tmp, ray, material, type);

    return (found_intersection && (type == IntersectedType::POINT_LIGHT ||
                                   type == IntersectedType::SPOT_LIGHT)) ||
           !found_intersection; // for directional
}

// L - ray from intersection point to light source
// V - ray from intersection point to camera
// H - vector between L and V
// D - distance to light source
glm::vec3 Scene::blinnPhong(const math::Intersection & nearest,
                            const Material * material,
                            const Camera & camera)
{    
    // ambient
    glm::vec3 ambient = material->albedo * AMBIENT;

    glm::vec3 diffuse(0, 0, 0);
    glm::vec3 specular(0, 0, 0);
    
    // directional light
    for (int i = 0, size = d_lights.size(); i != size; ++i)
    {
        glm::vec3 L = -glm::normalize(d_lights[i].direction);

        if (!isVisible(nearest, L)) continue;

        // diffuse
        float cosa = fmax(0, glm::dot(nearest.normal, L));

        diffuse += d_lights[i].color *
                   cosa *
                   material->albedo;
            
        // no specular for directional light
    }

    // point light
    for (int i = 0, size = p_lights.size(); i != size; ++i)
    {
        glm::vec3 L = p_lights[i].position - nearest.point;
        float D = glm::length(L);
        L = glm::normalize(L);

        if (!isVisible(nearest, L)) continue;

        // diffuse
        float cosa = fmax(0, glm::dot(nearest.normal, L));

        float light_intensity = (p_lights[i].radius * p_lights[i].radius) /
            (D * D);

        diffuse += p_lights[i].material.albedo *
                   cosa *
                   material->albedo *
                   light_intensity;            
            
        // specular not depends on object color and distance to light source
        // (only light source color)
        glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
        glm::vec3 H = glm::normalize(V + L);
        float cosb = fmax(0, glm::dot(nearest.normal, H));

        specular += p_lights[i].material.albedo *
                    powf(cosb, material->glossiness) *
                    material->specular;
    }

    // spot light
    for (int i = 0, size = s_lights.size(); i != size; ++i)
    {
        glm::vec3 L = s_lights[i].position - nearest.point;
        float D = glm::length(L);
        L = glm::normalize(L);

        if (!isVisible(nearest, L)) continue;

        // diffuse
        // check if intersection point under spot light
        if (glm::dot(s_lights[i].direction, -L) >= 
            cosf(glm::radians(s_lights[i].angle / 2)))
        {
            float cosa = fmax(0, glm::dot(nearest.normal, L));
                        
            float light_intensity = (s_lights[i].radius * s_lights[i].radius) /
                (D * D);

            diffuse += s_lights[i].material.albedo *
                cosa *
                material->albedo *
                light_intensity;
        }
        // even if spot light don't illuminate object, specular exists
            
        // specular not depends on object color and distance to light source
        // (only light source color)
        glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
        glm::vec3 H = glm::normalize(V + L);
        float cosb = fmax(0, dot(nearest.normal, H));

        specular += s_lights[i].material.albedo *
                    powf(cosb, material->glossiness) * 
                    material->specular;
    }    

    glm::vec3 result = material->emission + ambient + diffuse + specular;

    return result;
}

glm::vec3 Scene::ToneMappingACES(const glm::vec3 & hdr) const
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
    glm::vec3 ldr = clamp(m2 * (a / b), 0.0f, 1.0f);

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
        
    std::vector<int> & pixels = win.getPixels();

    math::Intersection nearest;
    nearest.reset();

    math::Ray ray;
    ray.origin = camera.getPosition();

    // find CS corner points in WS
    glm::vec3 bottom_left_WS = camera.generateWorldPointFromCS(-1.0f, -1.0f);
    glm::vec3 bottom_right_WS = camera.generateWorldPointFromCS(1.0f, -1.0f);
    glm::vec3 top_left_WS = camera.generateWorldPointFromCS(-1.0f, 1.0f);

    glm::vec3 near_plane_right = bottom_right_WS - bottom_left_WS;
    glm::vec3 near_plane_top = top_left_WS - bottom_left_WS;

    for (int y = 0; y != height; ++y)
    {
        for (int x = 0; x != width; ++x)
        {
            // normalized [0; 1]
            glm::vec2 xy;
            xy.x = float(x + 0.5f) / width;
            xy.y = 1.0f - float(y + 0.5f) / height; // inversed

            ray.direction = (bottom_left_WS +
                             xy.x * near_plane_right +
                             xy.y * near_plane_top) - ray.origin;

            Material * material;
            IntersectedType type;

            if (findIntersection(nearest, ray, material, type))
            {
                glm::vec3 result_color(0.0f, 0.0f, 0.0f);

                if (type != IntersectedType::POINT_LIGHT &&
                    type != IntersectedType::SPOT_LIGHT)
                {
                    result_color = blinnPhong(nearest, material, camera);
                    result_color = camera.adjustExposure(result_color);
                    result_color = ToneMappingACES(result_color);                  
                    result_color = gammaCorrection(result_color);
                }
                else
                {
                    result_color = material->albedo;
                }                

                pixels[y * width + x] = RGB(result_color.z * 255,
                                            result_color.y * 255,
                                            result_color.x * 255);
            }
            else
            {
                pixels[y * width + x] = HEX_BLACK;
            }
        }
    }

    win.flush();
}

