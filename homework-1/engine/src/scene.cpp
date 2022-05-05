#include "scene.hpp"

Scene::Scene(Sphere sphere) : sphere(sphere)
{}

void Scene::render(Window & win)
{ 
    RECT client = win.getSize();
    int width = client.right - client.left;
    int height = client.bottom - client.top;
        
    std::vector<int> & pixels = win.getPixels();

    // orthographic: all rays collinear
    vec3 ray_direction(0, 0, 1);

    for (int y = 0; y != height; ++y)        
    {
        for (int x = 0; x != width; ++x)
        {
            // new coord system with (0, 0) in client area center
            int new_x = x - width / 2;
            int new_y = y - height / 2;

            vec3 ray_origin(new_x, new_y, -1);

            if (sphere.isIntersect(ray_origin, ray_direction))
            {
                pixels[y * width + x] = HEX_WHITE;
            }
            else
            {
                pixels[y * width + x] = HEX_BLACK;
            }
        }
    }

    win.flush();
}

