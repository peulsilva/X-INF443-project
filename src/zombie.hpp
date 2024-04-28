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

        bool is_alive = true;
        bool display_death_animation = true;

        void get_shot();

        vec3 looking_at();

        zombie(vec3 _position);

        zombie();

        zombie(const zombie& other);

        void walk(vec3 player_position);
};