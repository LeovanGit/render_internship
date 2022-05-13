#include "scene.hpp"

Scene::Scene(std::vector<ColoredSphere> c_spheres,
             std::vector<Plane> planes,
             std::vector<PointLight> p_lights) : 
             c_spheres(c_spheres),
             planes(planes),
             p_lights(p_lights)
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

    for (int i = 0, size = planes.size(); i != size; ++i)
    {
        //found_intersection |= planes[i].intersect(nearest, ray);
    }

    for (int i = 0, size = p_lights.size(); i != size; ++i)
    {
        found_intersection |= p_lights[i].intersect(nearest, ray);
    }

    return found_intersection;
}

// L - ray from intersection point to light source
// V - ray from intersection point to camera
// H - vector between L and V
// D - distance to light source
glm::vec3 Scene::blinnPhong(Intersection & nearest,
                            Camera & camera,
                            bool visibility)
{    
    // emission
    glm::vec3 result = nearest.material.emission;

    // ambient
    result += nearest.material.albedo * ambient;

    glm::vec3 diffuse(0, 0, 0);
    glm::vec3 specular(0, 0, 0);
    
    if (visibility == true)
    {
        for (int i = 0, size = p_lights.size(); i != size; ++i)
        {
            // diffuse
            glm::vec3 L = p_lights[i].sphere.origin - nearest.point;
            float D = glm::length(L);
            L = glm::normalize(L);
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

                if (nearest.type == SPHERE ||
                    nearest.type == PLANE)
                {
                    result_color = blinnPhong(nearest, camera, true);
                }
                else if (nearest.type == LIGHT)
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

