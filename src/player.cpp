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
    std::unordered_map<std::string, zombie>& _zombies,
    std::unordered_map<std::string, vec3>& obstacles
){
    
    curr_weapon = weapon(handgun);

    is_aiming = false;

    velocity = constants::MAX_VELOCITY;
    player_direction = {0,0,0};

    zombies = &_zombies;
    obstacle_positions = obstacles;

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
    for (auto [n,obstacle] : obstacle_positions)
        direction = collide_with_object(obstacle, direction, n);

    if (std::abs(position.x + direction.x) >= constants::ACESSIBLE_AREA)
        direction.x = 0;

    if (std::abs(position.z + direction.z) >= constants::ACESSIBLE_AREA)
        direction.z = 0;

    if (std::abs(obstacle_positions["house"].z - position.z - direction.z) <= 8 && std::abs(obstacle_positions["house"].x - position.x) <= 4)
        direction.z = 0;

    if (std::abs(obstacle_positions["house"].x - position.x - direction.x) <= 4 && std::abs(obstacle_positions["house"].z - position.z) <= 8)
        direction.x = 0;

    if (norm(direction) > 0)
        direction = direction/norm(direction)*delta_s;
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
            n_shots ++;
            for (auto&[name, _zombie]: *zombies){

                // checking horizontal hit 
                if (!_zombie.is_alive)
                    continue;
                vec3 v1 = utils::remove_y_direction(_zombie.position - position);
                vec3 v2 = utils::remove_y_direction(looking_at());
                float dist = norm(_zombie.position - position);
                float angle = acos(dot(v1, v2)) * 180/ constants::PI;
                
                if (angle < 10/dist){
                    correct_shots++;
                    int damage = curr_weapon.get_damage(_zombie.position, position);
                    _zombie.get_shot(damage);
                    if (_zombie.is_alive)
                        points+= 10;

                    else {
                        points+= 50; 
                        n_kills++;
                    }
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

vec3 player::collide_with_object(vec3 obj_position, vec3 moving_direction, std::string obj_name){
    vec3 d_ab = obj_position - position;
    double prev_norm = norm(moving_direction);
    if (obj_name.compare("car") == 0){
        if (norm(d_ab) < 3){
            moving_direction = restrict_movement(obj_position, moving_direction);
        }
    }

    else if (obj_name.substr(0,5).compare("house") == 0){
        if (pow(d_ab.x/6., 100) + pow(d_ab.z/9. ,100) < 1){
            
            moving_direction = restrict_movement(obj_position, moving_direction);

        }
    }


    else if (obj_name.substr(0,4).compare("rock") == 0){
        if (norm(d_ab ) < 0.6){
            moving_direction = restrict_movement(obj_position, moving_direction);
        }
    }

    else if (norm(d_ab) < 1){
        moving_direction = restrict_movement(obj_position, moving_direction);
    }
    
    return moving_direction;
}


void player::take_hit(){

    health -= 30;
    if (health > 0)
        lists.take_hit = true;

    if (health < 0){
        is_alive = false;
        camera->camera_model.position_camera -= {0, 0.7, 0};
        lists.death = true;
    }
}

void player::get_weapon(
    std::vector<std::pair<weapon, vec3>> & weapons,
    std::vector<vec3> & weapon_pos
){
    int delete_idx = 0;
    bool is_near_any = false;
    for (auto& [w, pos] : weapons){
        if (norm(pos - position) < 2){
            is_near_any = true;
            w.object.material.phong.specular_exponent = 64;
            w.object.material.phong.specular = 0.3;
            w.object.material.phong.ambient = 0.3;
            w.object.material.phong.diffuse = 0.6;
            curr_weapon = w;
            break;
        }
        delete_idx++;
    }

    // Decrement here
    if (is_near_any){
        weapons.erase(weapons.begin() + delete_idx);
        weapon_pos.erase(weapon_pos.begin() + delete_idx) ; 
    }
    

    return;
}

void player::heal(
    std::vector<medicine>& medicines, 
    std::vector<vec3>& medicine_pos
){
    int delete_idx = 0;
    bool is_near_any = false;

    for (int i = 0; i < medicines.size(); i ++){

        vec3 pos = medicine_pos[i];

        if (norm(pos - position) < 2){
            is_near_any = true;
            health = 100;
            break;
        }
        delete_idx++;
    }

    // Decrement here
    if (is_near_any){
        medicines.erase(medicines.begin() + delete_idx);
        medicine_pos.erase(medicine_pos.begin() + delete_idx) ; 
    }
    

    return;
}

void player::set_fps(int _fps){
    fps = _fps;
    hit_timeout = 1.6*fps;
    // velocity = constants::MAX_VELOCITY * pow(60/((double)fps), 1/2.);

    curr_weapon.set_fps(fps);
}