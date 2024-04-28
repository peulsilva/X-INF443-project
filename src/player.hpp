#pragma once
#include "cgp/cgp.hpp"
#include "camera.hpp"
#include "utils.hpp"

using namespace cgp;

class player{
    public: 
        vec3 position;
        camera_controller_first_person_euler *camera;

        float velocity;

        mesh_drawable weapon;

        vec3 player_direction;
        vec3 camera_position;

        bool is_aiming;

        player(vec3 _position, camera_controller_first_person_euler& _camera);

        player();

        void move();

        void draw(const environment_structure& env, bool wireframe);

        void handle_mouse_click();

    private:
        vec3 remove_y_direction(vec3 v);
};