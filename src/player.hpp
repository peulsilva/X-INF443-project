#include "cgp/cgp.hpp"
#include "camera.hpp"
#include "constants.hpp"

using namespace cgp;

class player{
    public: 
        vec3 position;
        camera_controller_first_person_euler *camera;

        float velocity;


        vec3 player_direction;
        vec3 camera_position;

        player(vec3 _position, camera_controller_first_person_euler& _camera);

        player();

        void move();

    private:
        vec3 remove_z_direction(vec3 v);
};