#include "player.hpp"

using namespace cgp;

player::player(
    vec3 _position, 
    camera_controller_first_person_euler& _camera
){
    velocity = constants::MAX_VELOCITY;
    player_direction = {0,0,0};

    position = _position;
    camera = &_camera;

    camera_position = _position += vec3{0,0, 1.5};

    camera->camera_model.position_camera = camera_position;
}

player::player(){}


/**
 * Removes the z-direction component from a 3D vector.
 * 
 * This method takes a 3D vector as input and returns a new vector
 * with the z-component removed. The z-component of the input vector
 * is set to zero, effectively projecting the vector onto the xy-plane.
 * The resulting vector is then normalized to maintain its direction.
 * 
 * @param v The input 3D vector to remove the z-direction from.
 * @return A new 3D vector with the z-component removed and normalized.
 */
vec3 player::remove_z_direction(vec3 v){
    vec3 v_without_z = mat3{{1,0,0},{0,1,0}, {0,0,0}} * v;
    if (norm(v_without_z) < 1e-3)
        return {0,0,0};
    return normalize(v_without_z);
}

void player::move(){
    auto& inputs = camera->inputs;


    float const dt = inputs->time_interval;
    float delta_s = velocity*dt;

    vec3 direction = {0,0,0};

    if (inputs->keyboard.is_pressed(GLFW_KEY_A)){
        direction -= delta_s * remove_z_direction(camera->camera_model.right());
    }

    if (inputs->keyboard.is_pressed(GLFW_KEY_D)){
        direction += delta_s * remove_z_direction(camera->camera_model.right());
    }
    
    if (inputs->keyboard.is_pressed(GLFW_KEY_W)){
        direction += delta_s* remove_z_direction(camera->camera_model.front());
    }
    
    if (inputs->keyboard.is_pressed(GLFW_KEY_S)){
        direction -= delta_s * remove_z_direction(camera->camera_model.front());
    }


    position += direction;
    camera_position+= direction;

    camera->camera_model.position_camera = camera_position;
}