#pragma once
#include "cgp/cgp.hpp"


#include "character_loader/character_loader.hpp"
#include "animated_character/animated_character.hpp"
#include "effects/effects.hpp"
#include "constants.hpp"

using namespace cgp;

class zombie {
    public:
        vec3 position;
        float speed = constants::ENEMY_SPEED;
        character_structure character;
        effect_walking_structure effect_walking;
        bool first_iteration = true;

        zombie(vec3 _position);

        zombie();

        zombie(const zombie& other);

        void walk(vec3 player_position);
};