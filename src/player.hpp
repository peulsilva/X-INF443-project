#pragma once
#include "cgp/cgp.hpp"
#include "camera.hpp"
#include "utils.hpp"
#include "zombie.hpp"
#include "weapon.hpp"
#include "medicine.hpp"
#include <unordered_map>

using namespace cgp;

class player{
    public: 
        vec3 position;
        camera_controller_first_person_euler *camera;

        float velocity;
        float health = 100;
        bool is_alive = true;

        int fps = 60;

        int points= 0 ;
        int n_kills = 0;

        int n_shots = 0;
        int correct_shots = 0;

        bool use_hit_timeout_counter = false;
        int hit_timeout = 1.6*fps;
        int timeout_counter = 0;

        // mesh_drawable weapon;
        weapon curr_weapon;

        vec3 player_direction;
        vec3 camera_position;

        std::unordered_map<std::string, zombie>* zombies;
        std::unordered_map<std::string, vec3> obstacle_positions;

        bool is_aiming;

        player(vec3 _position, camera_controller_first_person_euler& _camera, std::unordered_map<std::string, zombie>& _zombies, std::unordered_map<std::string, vec3>& obstacles);

        player();

        void move();

        void draw(const environment_structure& env, bool wireframe);

        void handle_mouse_click();

        vec3 looking_at();

        vec3 collide_with_zombie(vec3 moving_direction);

        vec3 restrict_movement(vec3 zombie_pos, vec3 moving_direction);
        vec3 collide_with_object(vec3 house_pos, vec3 moving_direction);

        void set_fps(int _fps);

        void take_hit();

        void get_weapon(std::vector<std::pair<weapon, vec3>> & weapons, std::vector<vec3> & weapon_pos);

        void heal(std::vector<medicine>& medicines, std::vector<vec3>& medicine_pos);

    private:
        vec3 remove_y_direction(vec3 v);
};