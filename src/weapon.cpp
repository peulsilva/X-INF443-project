#include "weapon.hpp"
#include "unordered_map"

using namespace cgp;

#include "audio_controller.hpp"


weapon::weapon(){}

weapon::weapon(weapon_type _type){


    type = _type;

    if (type == rifle){
        timeout_factor = 32/60.;
        object.initialize_data_on_gpu(
            mesh_load_file_obj("assets/Ak_47/Ak-47.obj")
        );
        

        damage = 50;

        bullets_in_clip = 16;
        total_bullets = 48;
        max_bullets_in_clip = bullets_in_clip;
    }

    if (type == handgun){
        timeout_factor = 20/60.;
        object.initialize_data_on_gpu(
            mesh_load_file_obj("assets/M9.obj")
        );

        damage = 30;

        bullets_in_clip = 15;
        total_bullets = 45;
        max_bullets_in_clip = bullets_in_clip;
    }

    if (type == shotgun){
        timeout_factor = 65/60.;

        damage = 100;

        object.initialize_data_on_gpu(
            mesh_load_file_obj("assets/shotgun/sg.obj")
        );

        bullets_in_clip = 8;
        total_bullets = 32;
        max_bullets_in_clip = bullets_in_clip;

    }

    if (type == smg){
        timeout_factor = 7/60.;

        damage = 10;

        object.initialize_data_on_gpu(
            mesh_load_file_obj("assets/ump47.obj")
        );


        bullets_in_clip = 30;
        total_bullets = 90;
        max_bullets_in_clip = bullets_in_clip;
    }
    object.material.color = 0.1f * vec3{1, 1, 1};

}

std::string weapon::get_weapon_name(){
    return weapon_name_map[type];
}

bool weapon::shoot(){
    if (counter < timeout)
        return false;

    if (bullets_in_clip == 0){
        lists.empty_clip = true;
        return false;
    }
    else{

        if (type == rifle){
            lists.shoot_rifle = true;
        }

        else if (type == handgun){
            lists.shoot_handgun = true;
        }

        else if (type == shotgun){
            lists.shoot_shotgun = true;
        }

        else if (type == smg){
            lists.shoot_smg = true;
        }

        bullets_in_clip-= 1;

        counter = 0;
        return true;
    }
}

int weapon::get_damage(vec3 zombie_pos, vec3 player_pos){
    if (type != shotgun)
        return damage;

    vec3 d_ab = zombie_pos - player_pos;

    return damage/norm(d_ab) * 6;
}

void weapon::set_fps(int fps){
    timeout = timeout_factor * fps;
}

void weapon::reload(){
    if (bullets_in_clip == max_bullets_in_clip)
        return;

    if (total_bullets == 0)
        return;

    if (total_bullets > max_bullets_in_clip - bullets_in_clip){

        total_bullets -= max_bullets_in_clip - bullets_in_clip;
        bullets_in_clip = max_bullets_in_clip;
    }

    else if (total_bullets < max_bullets_in_clip - bullets_in_clip){
        bullets_in_clip = bullets_in_clip + total_bullets;
        total_bullets = 0;
    }
    
    switch (type)
    {
        case rifle:
            lists.reload_rifle = true;

            counter = -30;
            break;
        
        case handgun:
            lists.reload_handgun = true;
            counter = -60;
            break;

        case shotgun:
            lists.reload_shotgun = true;
            counter = -160;
            break;

        default:
            lists.reload_rifle = true;
            counter = -30;
            break;
    }
    
}

void weapon::draw(
    const environment_structure& env, 
    camera_controller_first_person_euler* camera,
    bool wireframe,
    bool is_aiming
){
    counter += 1;
    if (type == rifle){
        vec3 e1 = normalize(vec3{-1,0,0});
        object.model.rotation = rotation_transform::from_frame_transform(
            e1, 
            {0,0,1}, 
            camera->camera_model.front(), 
            -camera->camera_model.right()
        );
        
        object.model.translation = !is_aiming ? 
            camera->camera_model.position() - 0.6 * camera->camera_model.up() + 0.1 * camera->camera_model.right() + 0.1 * camera->camera_model.front()
            : camera->camera_model.position() - 0.57 * camera->camera_model.up() +  0.034 * camera->camera_model.right() + 0.1 * camera->camera_model.front();
        
        object.model.set_scaling(5e-3f);
    }

    else if (type == handgun){
        vec3 e1 = normalize(vec3{-1,0,0});
        object.model.rotation = rotation_transform::from_frame_transform(
            {1,0,0}, 
            {0,0,1}, 
            camera->camera_model.front(), 
            -camera->camera_model.up()
        );
        
        object.model.translation = !is_aiming ? 
            camera->camera_model.position() - 0.15 * camera->camera_model.up() + 0.1 * camera->camera_model.right() + 0.4 * camera->camera_model.front()
            : camera->camera_model.position() - 0.1 * camera->camera_model.up() + 1e-3 * camera->camera_model.right() + 0.4 * camera->camera_model.front();
        
        object.model.set_scaling(3e-2f);
    }

    else if (type == shotgun){
        object.model.rotation = rotation_transform::from_frame_transform(
            {1,0,0}, 
            {0,0,1}, 
            -camera->camera_model.front(), 
            -camera->camera_model.right()
        );
        
        object.model.translation = !is_aiming ? 
            camera->camera_model.position() - 0.15 * camera->camera_model.up() + 0.1 * camera->camera_model.right() + 0.6 * camera->camera_model.front()
            : camera->camera_model.position() - 0.05* camera->camera_model.up()  -2e-2* camera->camera_model.right() + 0.5 * camera->camera_model.front();
        
        object.model.set_scaling(3e-1f);
    }
    
    else if (type == smg){
        object.model.rotation = rotation_transform::from_frame_transform(
            {1,0,0}, 
            {0,1,0}, 
            camera->camera_model.front(), 
            camera->camera_model.up()
        );
        
        object.model.translation = !is_aiming ? 
            camera->camera_model.position() - 0.45 * camera->camera_model.up() + 0.3 * camera->camera_model.right() + 0.6 * camera->camera_model.front()
            : camera->camera_model.position() - 0.35 * camera->camera_model.up() + 0.11 * camera->camera_model.right() + 0.4 * camera->camera_model.front();
        
        object.model.set_scaling(1e-1f);
    }


    if (wireframe)
        draw_wireframe(object, env);

    else
        cgp::draw(object, env);

}

void weapon::draw_on_scene(
    const environment_structure& env,
    vec3 position
){
    if (type == rifle){
        vec3 e1 = normalize(vec3{-1,0,0});
        object.model.rotation = rotation_transform::from_frame_transform(
            e1, 
            {0,0,1}, 
            {0,0,1}, 
            vec3{1,0,0}
        );
        
        object.model.translation = {position.x, 0.7, position.z};

        // object.model = 100;
        
        object.model.set_scaling(3e-3f);
    }

    else if (type == handgun){
        vec3 e1 = normalize(vec3{-1,0,0});
        object.model.rotation = rotation_transform::from_frame_transform(
            {1,0,0}, 
            {0,0,-1}, 
            {0,0,1}, 
            {0,1,0}
        );
        
        object.model.translation = {position.x, 0.8, position.z};
        
        object.model.set_scaling(3e-2f);
    }

    else if (type == shotgun){
        object.model.rotation = rotation_transform::from_frame_transform(
            {1,0,0}, 
            {0,0,1}, 
            {0,0,1}, 
            {-1,0,0}
        );
        
        object.model.translation = {position.x, 0.8, position.z};
        
        object.model.set_scaling(3e-1f);
    }

    else if (type == smg){
        object.model.rotation = rotation_transform::from_frame_transform(
            {1,0,0}, 
            {0,1,0}, 
            {0,0,1}, 
            {0,1,0}
        );
        
        object.model.translation = {position.x, 0.8, position.z};
        object.model.set_scaling(1e-1f);
    }
    object.material.phong.specular_exponent = 100;
    object.material.phong.specular = 1;
    object.material.phong.ambient = 0.4;
    object.material.phong.diffuse = 1;


    cgp::draw(object, env);
}


weapon_type weapon::choose_random_weapon(){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Generate a random number between 0 and 3 (number of enum values - 1)
    int randomIndex = std::rand() % 4; // 4 is the number of enum values

    // Convert the random number to a weapon type
    return static_cast<weapon_type>(randomIndex);
}