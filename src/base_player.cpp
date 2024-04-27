#include "base_player.hpp"

base_player::base_player(vec3 _position){
    position = _position;
    velocity = {0, 0, 0};
    direction = {1.0, 0};
}