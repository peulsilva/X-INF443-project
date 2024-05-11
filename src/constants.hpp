
#define CONSTANTS_HPP

namespace constants {
    // Define constants here
    const float PI = 4* atan(1);

    // speed
    const float STEP_SIZE = 0.1;
    const float MAX_VELOCITY = 6;
    const float ENEMY_SPEED_WALKING= 0.02;
    const float ENEMY_SPEED_RUNNING= 0.06;
    
    const int MAX_ZOMBIES_ON_SCREEN = 6;
    const int WORLD_SIZE = 100;
    const int ACESSIBLE_AREA = 0.95* WORLD_SIZE;
    const int FOG_DEPTH = 25;

    // spawns 
    const int MAX_SPAWN_WEAPONS = 5;
    const int MAX_SPAWN_MEDICINE = 3;
    const int MAX_DIST_SPAWN_ZOMBIES = 20;
    const float PROBABILITY_SPAWN_ZOMBIES = 1/200.;
    const float PROBABILITY_SPAWN_WEAPONS = 1/500.;
    const float PROBABILITY_SPAWN_MEDICINES = 1/500.;

    // minimap
    const int MINIMAP_WIDTH = 50;
    const int MINIMAP_HEIGHT = 25;

    // debug

    const bool SHOULD_SPAWN_ZOMBIES = true;
    const bool SHOULD_SPAWN_WEAPONS = true;
    const bool SHOULD_SPAWN_MEDICINES = true;
}
