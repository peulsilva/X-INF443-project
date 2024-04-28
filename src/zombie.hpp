
#include "cgp/cgp.hpp"


#include "character_loader/character_loader.hpp"
#include "animated_character/animated_character.hpp"
#include "effects/effects.hpp"

using namespace cgp;

class zombie {
    public:
        vec3 position;

        character_structure character;
        effect_walking_structure effect_walking;

        zombie(vec3 _position);

        zombie();

        void walk(vec3 player_position);
};