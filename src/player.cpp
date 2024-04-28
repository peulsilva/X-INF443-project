#include "player.hpp"

#include "constants.hpp"

using namespace cgp;

player::player(
    vec3 _position, 
    camera_controller_first_person_euler& _camera
){
    
    weapon.initialize_data_on_gpu(
        mesh_load_file_obj("assets/Ak_47/Ak-47.obj")
    );

    // weapon.initialize_data_on_gpu(
    //     mesh_load_file_obj("assets/M9.obj")
    // );

    is_aiming = false;

    weapon.material.color = 0.3f * vec3{1, 1, 1};

    velocity = constants::MAX_VELOCITY;
    player_direction = {0,0,0};

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
    auto& inputs = camera->inputs;


    float const dt = inputs->time_interval;
    float delta_s = velocity*dt;

    vec3 direction = {0,0,0};

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


    position += direction;
    camera_position+= direction;

    camera->camera_model.position_camera = camera_position;
}

void player::draw(
    const environment_structure& env, 
    bool wireframe
){

    vec3 e1 = normalize(vec3{-1,0,0});
    weapon.model.rotation = rotation_transform::from_frame_transform(
        e1, 
        {0,0,1}, 
        camera->camera_model.front(), 
        -camera->camera_model.right()
    );
    
    weapon.model.translation = !is_aiming ? 
        camera->camera_model.position() - 0.6 * camera->camera_model.up() + 0.1 * camera->camera_model.right()
        : camera->camera_model.position() - 0.57 * camera->camera_model.up() +  0.03 * camera->camera_model.right();
    
    weapon.model.set_scaling(5e-3f);
    if (wireframe)
        draw_wireframe(weapon, env);

    else
        cgp::draw(weapon, env);
}

void player::handle_mouse_click(){
    auto& inputs = camera->inputs;
    bool const click_right = inputs->mouse.click.right;
    bool const click_left = inputs->mouse.click.left;

    if (click_right)
        is_aiming = true;

    else
        is_aiming = false;
}