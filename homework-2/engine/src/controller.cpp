#include "controller.hpp"
#include "plane.hpp"

void Controller::init(Scene * scene)
{
    this->scene = scene;

    mouse = glm::vec2(0);
    fixed_mouse = glm::vec2(0);
    delta_fixed_mouse = glm::vec2(0);

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
        RECT client_area = win.getClientSize();
        int width = client_area.right - client_area.left;
        int height = client_area.bottom - client_area.top;

        glm::vec2 xy;
        xy.x = 2.0f * mouse.x / width - 1.0f;           
        xy.y = 1.0f - 2.0f * mouse.y / height; // reversed

        math::Ray ray;
        ray.origin = camera.getPosition();

        glm::vec4 dir_cs(xy.x, xy.y, 1.0f, 1.0f);
        glm::vec4 dir_ws = camera.getViewProjInv() * dir_cs;

        ray.direction = glm::normalize((glm::vec3(dir_ws) / dir_ws.w) -
                                       camera.getPosition());

        if (!current_object.is_grabbed)
        {        
            Scene::IntersectionQuery iq;
            iq.nearest.reset();
            iq.mover = &current_object.mover;

            if (scene->findIntersection(ray, iq))
            {
                current_object.grabbed_point = iq.nearest.point;
                if (iq.mover->get()) current_object.is_grabbed = true;
            }
        }
        else
        {
            // movement plane
            math::Plane plane(camera.getForward(),
                              current_object.grabbed_point);

            math::Intersection intersection;
            intersection.reset();

            plane.intersect(intersection, ray);

            current_object.mover->move(intersection.point -
                                       current_object.grabbed_point);
            current_object.grabbed_point = intersection.point;
        }
    } 
    else
    {
        current_object.is_grabbed = false;
        current_object.mover.reset();
    }
}

void Controller::calcMouseMovement(LPARAM lParam)
{
    mouse = glm::vec2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    if (keys_log[KEY_LMOUSE])
    {
        delta_fixed_mouse.x = mouse.x - fixed_mouse.x;
        delta_fixed_mouse.y = mouse.y - fixed_mouse.y;
    }
}
