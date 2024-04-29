#pragma once
#include "cgp/cgp.hpp"
#include "camera.hpp"
#include "utils.hpp"
#include "zombie.hpp"

using namespace cgp;

class player{
    public: 
        vec3 position;
        camera_controller_first_person_euler *camera;

        float velocity;
        float health = 100;
        bool is_alive = true;

        bool use_hit_timeout_counter = false;
        int hit_timeout = 100;
        int timeout_counter = 0;

        mesh_drawable weapon;

        vec3 player_direction;
        vec3 camera_position;

        std::map<std::string, zombie>* zombies;

        bool is_aiming;

        player(vec3 _position, camera_controller_first_person_euler& _camera, std::map<std::string, zombie>& _zombies);

        player();

        void move();

        void draw(const environment_structure& env, bool wireframe);

        void handle_mouse_click();

        vec3 looking_at();

        vec3 collide_with_zombie(vec3 moving_direction);

        vec3 restrict_movement(vec3 zombie_pos, vec3 moving_direction);

        void take_hit();

    private:
        vec3 remove_y_direction(vec3 v);
};