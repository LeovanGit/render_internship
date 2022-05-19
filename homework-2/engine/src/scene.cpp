#include "scene.hpp"

Scene::Scene(std::vector<ColoredSphere> c_spheres,
             std::vector<ColoredPlane> c_planes,
             std::vector<Cube> c_cubes,
             std::vector<PointLight> p_lights,
             std::vector<DirectionalLight> d_lights,
             std::vector<SpotLight> s_lights) :
             c_spheres(c_spheres),
             c_planes(c_planes),
             c_cubes(c_cubes),             
             p_lights(p_lights),
             d_lights(d_lights),
             s_lights(s_lights)
{}

// iterate over all objects and find the nearest intersection
bool Scene::findIntersection(Intersection & nearest, Ray & ray)
{
    nearest.reset();
    
    bool found_intersection = false;

    for (int i = 0, size = c_spheres.size(); i != size; ++i)
    {
        found_intersection |= c_spheres[i].intersect(nearest, ray);
    }

    for (int i = 0, size = c_planes.size(); i != size; ++i)
    {
        found_intersection |= c_planes[i].intersect(nearest, ray);
    }

    for (int i = 0, size = c_cubes.size(); i != size; ++i)
    {
        found_intersection |= c_cubes[i].intersect(nearest, ray);
    }

    for (int i = 0, size = p_lights.size(); i != size; ++i)
    {
        found_intersection |= p_lights[i].intersect(nearest, ray);
    }

    for (int i = 0, size = d_lights.size(); i != size; ++i)
    {
        found_intersection |= d_lights[i].intersect(nearest, ray);
    }

    for (int i = 0, size = s_lights.size(); i != size; ++i)
    {
        found_intersection |= s_lights[i].intersect(nearest, ray);
    }

    return found_intersection;
}

bool Scene::isVisible(Intersection & nearest, glm::vec3 & dir_to_light)
{
    Ray ray;
    ray.direction = dir_to_light;
    ray.origin = nearest.point + delta * nearest.normal;

    Intersection tmp;
    tmp.reset();

    if (findIntersection(tmp, ray) && tmp.type == LIGHT ||
        !findIntersection(tmp, ray)) // for directional
        return true;

    return false;
}

// L - ray from intersection point to light source
// V - ray from intersection point to camera
// H - vector between L and V
// D - distance to light source
glm::vec3 Scene::blinnPhong(Intersection & nearest,
                            Camera & camera)
{    
    // emission
    glm::vec3 result = nearest.material.emission;

    // ambient
    result += nearest.material.albedo * ambient;

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
            nearest.material.albedo;
            
        // specular not depends on object color and distance to light source
        // (only light source color)
        L = glm::normalize(d_lights[i].sphere.origin - nearest.point);

        glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
        glm::vec3 H = glm::normalize(V + L);
        float cosb = fmax(0, dot(nearest.normal, H));

        specular += d_lights[i].color *
            (float)pow(cosb, nearest.material.glossiness) * 
            nearest.material.specular;
    }

    // point light
    for (int i = 0, size = p_lights.size(); i != size; ++i)
    {
        glm::vec3 L = p_lights[i].sphere.origin - nearest.point;
        float D = glm::length(L);
        L = glm::normalize(L);

        if (!isVisible(nearest, L)) continue;

        // diffuse
        float cosa = fmax(0, glm::dot(nearest.normal, L));

        float light_intensity = (p_lights[i].radius * p_lights[i].radius) /
            (D * D);

        diffuse += p_lights[i].color *
            cosa *
            nearest.material.albedo *
            light_intensity;            
            
        // specular not depends on object color and distance to light source
        // (only light source color)
        glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
        glm::vec3 H = glm::normalize(V + L);
        float cosb = fmax(0, dot(nearest.normal, H));

        specular += p_lights[i].color *
            (float)pow(cosb, nearest.material.glossiness) * 
            nearest.material.specular;
    }

    // spot light
    for (int i = 0, size = s_lights.size(); i != size; ++i)
    {
        glm::vec3 L = s_lights[i].sphere.origin - nearest.point;
        float D = glm::length(L);
        L = glm::normalize(L);

        if (!isVisible(nearest, L)) continue;

        // diffuse
        // check if intersection point under spot light
        if (glm::dot(s_lights[i].direction, -L) < 
            cos(glm::radians(s_lights[i].angle / 2))) continue;

        float cosa = fmax(0, glm::dot(nearest.normal, L));
                        
        float light_intensity = (s_lights[i].radius * s_lights[i].radius) /
            (D * D);

        diffuse += s_lights[i].color *
            cosa *
            nearest.material.albedo *
            light_intensity;            
            
        // specular not depends on object color and distance to light source
        // (only light source color)
        glm::vec3 V = glm::normalize(camera.getPosition() - nearest.point);
        glm::vec3 H = glm::normalize(V + L);
        float cosb = fmax(0, dot(nearest.normal, H));

        specular += s_lights[i].color *
                    (float)pow(cosb, nearest.material.glossiness) * 
                    nearest.material.specular;
    }    

    result += diffuse + specular;

    // normalize
    result.x = fmin(1.0f, result.x);
    result.y = fmin(1.0f, result.y);
    result.z = fmin(1.0f, result.z);

    return result;
}

void Scene::render(Window & win, Camera & camera)
{ 
    RECT client = win.getClientSize();
    int width = client.right - client.left;
    int height = client.bottom - client.top;
        
    std::vector<int> & pixels = win.getPixels();

    Intersection nearest;
    nearest.reset();

    Ray ray;
    ray.origin = camera.getPosition();

    for (int y = 0; y != height; ++y)
    {
        for (int x = 0; x != width; ++x)
        {
            // new coord system with (0, 0) in client area center
            // and normalized: [-1; 1]
            glm::vec2 xy;
            xy.x = 2.0f * x / width - 1.0f;           
            xy.y = 1.0f - 2.0f * y / height; // reversed

            // dirCS.z = 1.0f -> on near plane 
            //         = 0.0f -> on far plane
            // (versa for not reversed depth)
            glm::vec4 dir_cs(xy.x, xy.y, 1.0f, 1.0f);
            glm::vec4 dir_ws = camera.getViewProjInv() * dir_cs;

            ray.direction = glm::normalize((glm::vec3(dir_ws) / dir_ws.w) -
                                            camera.getPosition());

            if (findIntersection(nearest, ray))
            {
                glm::vec3 result_color(0.0f, 0.0f, 0.0f);

                if (nearest.type != LIGHT)
                {
                    result_color = blinnPhong(nearest, camera);
                }
                else
                {
                    result_color = nearest.material.albedo;
                }

                pixels[y * width + x] = RGB(result_color.x * 255,
                                            result_color.y * 255,
                                            result_color.z * 255);
            }
            else
            {
                pixels[y * width + x] = HEX_BLACK;
            }
        }
    }

    win.flush();
}

