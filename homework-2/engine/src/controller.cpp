#include "controller.hpp"

void Controller::init(Scene * scene)
{
    this->scene = scene;

    mouse.x = 0;
    mouse.y = 0;

    delta_mouse.x = 0;
    delta_mouse.y = 0;

    fixed_mouse.x = 0;
    fixed_mouse.y = 0;

    delta_fixed_mouse.x = 0;
    delta_fixed_mouse.y = 0;

    for (int i = 0; i != KEYS_COUNT; ++i) keys_log[i] = false;
}

void Controller::initScene(const std::vector<Scene::Sphere> & spheres,
                           const std::vector<Scene::Plane> & planes,
                           const std::vector<Scene::Cube> & cubes,
                           const std::vector<Scene::DirectionalLight> & d_lights,
                           const std::vector<Scene::PointLight> & p_lights,
                           const std::vector<Scene::SpotLight> & s_lights)
{
    scene->spheres = spheres;
    scene->planes = planes;
    scene->cubes = cubes;
    scene->d_lights = d_lights;
    scene->p_lights = p_lights;
    scene->s_lights = s_lights;
}

void Controller::processInput(Camera & camera,
                              float delta_time,
                              Window & win)
{
    if (keys_log[KEY_W])
        camera.addWorldPosition(camera.getForward() *
                                delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_S])
        camera.addWorldPosition(camera.getForward() * 
                                delta_time * -MOVEMENT_SPEED);
    if (keys_log[KEY_D])
        camera.addWorldPosition(camera.getRight() *
                                delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_A])
        camera.addWorldPosition(camera.getRight() *
                                delta_time * -MOVEMENT_SPEED);
    if (keys_log[KEY_SPACE])
        camera.addWorldPosition(camera.getUp() *
                                delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_CTRL])
        camera.addWorldPosition(camera.getUp() *
                                delta_time * -MOVEMENT_SPEED);
    if (keys_log[KEY_LMOUSE])
    {
        glm::vec3 angles;
        angles.x = delta_fixed_mouse.y / 100 * ROTATIONAL_SPEED * delta_time;
        angles.y = delta_fixed_mouse.x / 100 * ROTATIONAL_SPEED * delta_time;
        angles.z = 0;

        camera.addRelativeAngles(angles);
    }
    if (keys_log[KEY_Q])
    {
        camera.addRelativeAngles(glm::vec3(0,
                                           0,
                                           ROTATIONAL_SPEED * delta_time));
    }
    if (keys_log[KEY_E])
    {
        camera.addRelativeAngles(glm::vec3(0,
                                           0,
                                           -ROTATIONAL_SPEED * delta_time));
    }
    if (keys_log[KEY_RMOUSE])
    {
    //     RECT client_area = win.getClientSize();
    //     int width = client_area.right - client_area.left;
    //     int height = client_area.bottom - client_area.top;

    //     glm::vec2 xy;
    //     xy.x = 2.0f * mouse.x / width - 1.0f;           
    //     xy.y = 1.0f - 2.0f * mouse.y / height; // reversed

    //     Intersection nearest;
    //     nearest.reset();

    //     Ray ray;
    //     ray.origin = camera.getPosition();

    //     glm::vec4 dir_cs(xy.x, xy.y, 1.0f, 1.0f);
    //     glm::vec4 dir_ws = camera.getViewProjInv() * dir_cs;

    //     ray.direction = glm::normalize((glm::vec3(dir_ws) / dir_ws.w) -
    //                                    camera.getPosition());
        
    //     if (scene->findIntersection(nearest, ray))
    //     {
            
    //     }
    }
}

void Controller::calcMouseMovement(LPARAM lParam)
{
    glm::vec2 new_mouse(GET_X_LPARAM(lParam),
                        GET_Y_LPARAM(lParam));

    delta_mouse.x = new_mouse.x - mouse.x;
    delta_mouse.y = new_mouse.y - mouse.y;

    mouse.x = new_mouse.x;
    mouse.y = new_mouse.y;

    if (keys_log[KEY_LMOUSE])
    {
        delta_fixed_mouse.x = mouse.x - fixed_mouse.x;
        delta_fixed_mouse.y = mouse.y - fixed_mouse.y;
    }
}
