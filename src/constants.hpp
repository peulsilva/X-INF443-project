
#define CONSTANTS_HPP

namespace constants {
    // Define constants here
    const float STEP_SIZE = 0.1;
    const float MAX_VELOCITY = 6;
    const float PI = 4* atan(1);
    const float ENEMY_SPEED_WALKING= 0.02;
    const float ENEMY_SPEED_RUNNING= 0.06;
    const int MAX_ZOMBIES_ON_SCREEN = 10;
    const int WORLD_SIZE = 100;
    const int MAX_SPAWN_WEAPONS = 20;
    const int FOG_DEPTH = 25;
    const int MAX_DIST_SPAWN_ZOMBIES = 20;
    const float PROBABILITY_SPAWN_ZOMBIES = 1/200.;
    const float PROBABILITY_SPAWN_WEAPONS = 1/500.;
}
