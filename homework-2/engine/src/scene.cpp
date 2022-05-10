#include "scene.hpp"

Scene::Scene(std::vector<ColoredSphere> c_spheres,
             std::vector<Light> lights) : 
             c_spheres(c_spheres),
             lights(lights)
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

    return found_intersection;
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
                pixels[y * width + x] = RGB(nearest.material.color.getX(),
                                            nearest.material.color.getY(),
                                            nearest.material.color.getZ());
            }
            else
            {
                pixels[y * width + x] = HEX_BLACK;
            }
        }
    }

    win.flush();
}

