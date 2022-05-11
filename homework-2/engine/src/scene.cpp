#include "scene.hpp"
#include "math.hpp"

Scene::Scene(std::vector<ColoredSphere> c_spheres,
             std::vector<PointLight> p_lights) : 
             c_spheres(c_spheres),
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
vec3 Scene::blinnPhong(Intersection & nearest,
                       vec3 V,
                       bool visibility)
{    
    // emission
    vec3 result = nearest.material.emission;

    // ambient
    float ambient = 0.1f;
    result = result + nearest.material.albedo * ambient;

    vec3 diffuse(0, 0, 0);
    vec3 specular(0, 0, 0);
    
    if (visibility == true)
    {
        for (int i = 0, size = p_lights.size(); i != size; ++i)
        {
            vec3 L = p_lights[i].sphere.origin - nearest.point;
            float D = L.getLength();
            L = L.getNormalized();
            float cosa = fmax(0, dot(nearest.normal, L));

            float light_intensity = (p_lights[i].radius * p_lights[i].radius) / 
                (D * D);

            diffuse = diffuse + p_lights[i].color * 
                                cosa *
                                nearest.material.albedo *
                                light_intensity;

            // specular not depends on object color and distance to light source
            // (only light source color)
            vec3 H = (V + L).getNormalized();
            float cosb = fmax(0, dot(nearest.normal, H));

            specular = specular + p_lights[i].color *
                                  pow(cosb, nearest.material.glossiness) * 
                                  nearest.material.specular;
        }
    }

    result = result + diffuse + specular;

    // normalize
    result.x = fmin(1.0f, result.x);
    result.y = fmin(1.0f, result.y);
    result.z = fmin(1.0f, result.z);

    return result;
}

void Scene::render(Window & win)
{ 
    RECT client = win.getClientSize();
    int width = client.right - client.left;
    int height = client.bottom - client.top;
        
    std::vector<int> & pixels = win.getPixels();

    // orthographic: all rays collinear
    Ray ray;
    ray.direction = vec3(0, 0, 1);

    Intersection nearest;
    nearest.reset();

    for (int y = 0; y != height; ++y)        
    {
        for (int x = 0; x != width; ++x)
        {
            // new coord system with (0, 0) in client area center
            int new_x = x - width / 2;
            int new_y = y - height / 2;

            ray.origin = vec3(new_x, new_y, -1);

            if (findIntersection(nearest, ray))
            {
                vec3 result_color(0.0f, 0.0f, 0.0f);

                if (nearest.type == SPHERE)
                {
                    vec3 camera = vec3(0, 0, -500);
                    vec3 V = (camera - nearest.point).getNormalized();
                    result_color = blinnPhong(nearest, V, true);
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

