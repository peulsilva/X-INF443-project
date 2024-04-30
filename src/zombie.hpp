#pragma once
#include "cgp/cgp.hpp"


#include "character_loader/character_loader.hpp"
#include "animated_character/animated_character.hpp"
#include "effects/effects.hpp"

using namespace cgp;

class zombie {
    public:
        vec3 position;
        float speed;
        character_structure character;
        effect_walking_structure effect_walking;
        bool first_iteration = true;

        std::string name;

        bool is_alive = true;
        bool display_death_animation = true;

        void get_shot();

        vec3 looking_at();

        zombie(vec3 _position, std::string _name);

        zombie();

        zombie(const zombie& other);

        void move(vec3 player_position, std::map<std::string, zombie>& other_zombies);

        vec3 collide_with_player(vec3 player_position, vec3 walking_position);

        vec3 collide_with_zombies(std::map<std::string, zombie> & all_zombies, vec3 walking_direction);

        vec3 restrict_movement(vec3 other_zombie_pos, vec3 moving_direction);
};
