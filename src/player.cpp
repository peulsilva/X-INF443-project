#include "player.hpp"

#include "constants.hpp"
#include "audio_controller.hpp"

#include <future>
#include <thread>
#include <chrono>
#include <iostream>
using namespace cgp;


player::player(
    vec3 _position, 
    camera_controller_first_person_euler& _camera,
    std::unordered_map<std::string, zombie>& _zombies
){
    
    curr_weapon = weapon(handgun);

    is_aiming = false;

    

    velocity = constants::MAX_VELOCITY;
    player_direction = {0,0,0};

    zombies = &_zombies;

    position = _position;
    camera = &_camera;

    camera_position = _position += vec3{0,1.5,0};

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


void player::move(){

    if (!is_alive)
        return;

    // hit timeout counter 
    timeout_counter += 1;
    auto& inputs = camera->inputs;


    float const dt = inputs->time_interval;
    float delta_s = velocity*dt;

    vec3 direction = {0,0,0};

    if (inputs->keyboard.is_pressed(GLFW_KEY_R)){
        curr_weapon.reload();
    }

    if (inputs->keyboard.is_pressed(GLFW_KEY_A)){
        direction -= delta_s * utils::remove_y_direction(camera->camera_model.right());
    }

    if (inputs->keyboard.is_pressed(GLFW_KEY_D)){
        direction += delta_s * utils::remove_y_direction(camera->camera_model.right());
    }
    
    if (inputs->keyboard.is_pressed(GLFW_KEY_W)){
        direction += delta_s* utils::remove_y_direction(camera->camera_model.front());
    }
    
    if (inputs->keyboard.is_pressed(GLFW_KEY_S)){
        direction -= delta_s * utils::remove_y_direction(camera->camera_model.front());
    }

    if (norm(direction) > 0){
        lists.footsteps = true;
    }
    else{
        lists.footsteps = false;
    }

    direction = collide_with_zombie(direction);
    position += direction;
    camera_position+= direction;

    camera->camera_model.position_camera = camera_position;
}

vec3 player::looking_at(){
    return camera->camera_model.front();
}

void player::draw(
    const environment_structure& env, 
    bool wireframe
){

    curr_weapon.draw(
        env,
        camera,
        wireframe,
        is_aiming
    );
}

void player::handle_mouse_click(){
    auto& inputs = camera->inputs;
    bool const click_right = inputs->mouse.click.right;
    bool const click_left = inputs->mouse.click.left;

    if (click_right)
        is_aiming = true;

    else
        is_aiming = false;

    if (click_left){
        bool is_shot = curr_weapon.shoot();

        if (is_shot){

            for (auto&[name, _zombie]: *zombies){

                // checking horizontal hit 
                vec3 v1 = utils::remove_y_direction(_zombie.position - position);
                vec3 v2 = utils::remove_y_direction(looking_at());
                float dist = norm(_zombie.position - position);
                float angle = acos(dot(v1, v2)) * 180/ constants::PI;
                
                if (angle < 10/dist){
                    int damage = curr_weapon.get_damage(_zombie.position, position);
                    _zombie.get_shot(damage); 
                }

                // TODO: check vertical hit 
            }
        }
        
    }
}

vec3 player::restrict_movement(vec3 zombie_pos, vec3 moving_direction){
    vec3 v_ab = utils::remove_y_direction(zombie_pos - position);

    float cos_angle = dot(v_ab, moving_direction);

    if (cos_angle < 0) // moving in the opposite direction
        return moving_direction;

    vec3 projection = cos_angle* v_ab;

    return moving_direction - projection;
}

vec3 player::collide_with_zombie(vec3 moving_direction){
    for (auto&[name, _zombie]: *zombies){
        vec3 d_ab = _zombie.position - position;

        if (norm(d_ab) < 1.7){
            moving_direction = restrict_movement(_zombie.position, moving_direction);

            if (timeout_counter > hit_timeout && _zombie.is_alive){
                take_hit();

                timeout_counter = 0;
            }
        }
        
    }
    return moving_direction;
}

void player::take_hit(){

    health -= 30;
    if (health > 0)
        lists.take_hit = true;

    std::cout << health << std::endl;
    if (health < 0){
        is_alive = false;
        camera->camera_model.position_camera -= {0, 0.7, 0};
        std::cout << camera->camera_model.position_camera << std::endl;
        lists.death = true;
    }
}