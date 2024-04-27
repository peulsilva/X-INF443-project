#include "cgp/cgp.hpp"

using namespace cgp;

class base_player{
    public:
        vec3 position;
        vec2 direction;
        vec3 velocity;

    base_player(vec3 position_);
};