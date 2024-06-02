
#define CONSTANTS_HPP
using namespace cgp;

namespace constants {
    // Define constants here
    const float PI = 4* atan(1);

    // speed
    const float STEP_SIZE = 0.1;
    const float MAX_VELOCITY = 6;
    const float ENEMY_SPEED_WALKING= 0.02;
    const float ENEMY_SPEED_RUNNING= 0.06;
    
    const int MAX_ZOMBIES_ON_SCREEN = 1;
    const int WORLD_SIZE = 100;
    const int ACESSIBLE_AREA = 0.95* WORLD_SIZE;
    const int FOG_DEPTH = 10000;
    const vec3 FOG_COLOR = 0.01*vec3{1,1,1};


    // spawns 
    const int MAX_SPAWN_WEAPONS = 3;
    const int MAX_SPAWN_MEDICINE = 3;
    const int MAX_DIST_SPAWN_ZOMBIES = 20;
    const int MAX_DIST_SHOW_ZOMBIES = 100;
    const float PROBABILITY_SPAWN_ZOMBIES = 1/200.;
    const float PROBABILITY_SPAWN_WEAPONS = 1/1000.;
    const float PROBABILITY_SPAWN_MEDICINES = 1/1000.;

    // minimap
    const int MINIMAP_WIDTH = 50;
    const int MINIMAP_HEIGHT = 25;

    // levels

    const int RATIO_ZOMBIES_PER_LEVEL = 5;

    // debug

    const bool SHOULD_SPAWN_ZOMBIES = true;
    const bool SHOULD_SPAWN_WEAPONS = true;
    const bool SHOULD_SPAWN_MEDICINES = true;
}
