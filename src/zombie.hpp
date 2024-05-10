#pragma once
#include "cgp/cgp.hpp"


#include "character_loader/character_loader.hpp"
#include "animated_character/animated_character.hpp"
#include "effects/effects.hpp"
#include <unordered_map>

using namespace cgp;

class zombie {
    public:
        vec3 position;
        float speed;
        character_structure character;
        effect_walking_structure effect_walking;
        bool first_iteration = true;

        int health = 100;

        bool is_running = false;

        std::string name;

        bool is_alive = true;
        bool was_hit = false;
        bool display_death_animation = true;

        bool show = true;

        vec3 looking_at();

        zombie(vec3 _position, std::string _name);

        zombie(vec3 _position, std::string _name, bool _show);

        zombie();

        zombie(const zombie& other);

        void get_shot(int weapon_damage);

        void move(vec3 player_position, std::unordered_map<std::string, zombie>& other_zombies);

        vec3 collide_with_player(vec3 player_position, vec3 walking_position);

        vec3 collide_with_zombies(std::unordered_map<std::string, zombie> & all_zombies, vec3 walking_direction);

        vec3 restrict_movement(vec3 other_zombie_pos, vec3 moving_direction);
};
