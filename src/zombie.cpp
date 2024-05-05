#include "zombie.hpp"
#include "constants.hpp"
#include "audio_controller.hpp"

using namespace cgp;

zombie::zombie(vec3 _position, std::string _name){
	name = _name;
    position = _position;

	speed = constants::ENEMY_SPEED;
    character = load_character_zombie();
	character.set_current_animation("Walk");
    effect_walking.root_position = position + vec3{0,0.9,0};
	
}

zombie::zombie(vec3 _position, std::string _name, bool _show){
	name = _name;
    position = _position;

	show = false;

	speed = constants::ENEMY_SPEED;
    character = load_character_zombie();
	character.set_current_animation("Walk");
    effect_walking.root_position = position + vec3{0,0.9,0};
	
}

zombie::zombie(const zombie& other) {
    position = other.position;
    character = other.character; // Assuming character_structure supports copy construction
    // character.animated_model.apply_transformation(position);
	name = other.name;

}

vec3 zombie::looking_at(){
	return vec3(sin(effect_walking.root_angle), 0.0f, cos(effect_walking.root_angle));
}


zombie::zombie(){}

void zombie::get_shot(int weapon_damage){
	if (!is_alive)
		return;


	health -= weapon_damage;

	if (health <=0 ){

		is_alive = false;
		character.set_current_animation("Death");   
		character.timer.t_periodic = 0;

		if (!was_hit){
		
			rotation_transform r = rotation_axis_angle(vec3(0.0f, 1.0f, 0.0f), effect_walking.root_angle);

			character.animated_model.apply_transformation({0,0,0},r );
		}
	}

	else{
		character.set_current_animation("Hit");

		character.timer.t_periodic = 0;

		std::cout << effect_walking.root_angle << std::endl;
		if (!was_hit){
			
			rotation_transform r = rotation_axis_angle(vec3(0.0f, 1.0f, 0.0f), effect_walking.root_angle);

			character.animated_model.apply_transformation({0,0,0},r );
		}
		was_hit = true;
	}


}

void zombie::move(
	vec3 player_position,
	std::unordered_map<std::string, zombie>& other_zombies
)
{   
	if (! show)
		return;
	if (!is_alive || was_hit){

		vec3 forward_direction = vec3(sin(effect_walking.root_angle), 0.0f, cos(effect_walking.root_angle));
		// rotation_transform r = rotation_axis_angle(vec3(0.0f, 1.0f, 0.0f), effect_walking.root_angle);

		// character.animated_model.apply_transformation({0,0,0},r );
		

		character.animated_model.skeleton.joint_matrix_global[0].apply_translation({position.x,0,position.z});
		

		if (character.timer.t_periodic > character.timer.event_period -0.04){
			if (!is_alive)
				display_death_animation = false;
			
			if (was_hit){
				was_hit = false;
				character.set_current_animation("Walk");
			}
		}
		
		for (unsigned int i = 1; i <  character.animated_model.skeleton.joint_matrix_local.size(); i++)
		{
			int parent_index = character.animated_model.skeleton.parent_index[i];
			character.animated_model.skeleton.joint_matrix_global[i] = character.animated_model.skeleton.joint_matrix_global[parent_index] * character.animated_model.skeleton.joint_matrix_local[i];
		}
		character.animated_model.skeleton.update_joint_matrix_global_to_local();
		return;
	}

    int rotate = 1;
	lists.zombie_moaning = true;
	// Calculate direction from zombie to player
	vec3 forward_direction = vec3(sin(effect_walking.root_angle), 0.0f, cos(effect_walking.root_angle));
	vec3 zombie_to_player = (player_position - position) - forward_direction;

	// Normalize the direction vector
	vec3 zombie_to_player_normalized = utils::remove_y_direction(zombie_to_player);

    if (norm(player_position - position + vec3{0,1,0}) < 2){
        rotate = 0;
    } 

	// Calculate the angle between the forward direction of the zombie and the direction to the player
	float angle = acos(zombie_to_player_normalized.z);

	// std::cout << angle << std::endl;
	if (zombie_to_player_normalized.x < 0)
		angle *= -1; 

	// Adjust the angle gradually to smoothly turn towards the player
	float angleSpeed = 0.1f * rotate;

	if (abs(angle - effect_walking.root_angle) > angleSpeed) {
		float rotation_dir = angle - effect_walking.root_angle > 0.0f ? 1.0f : -1.0f;
		effect_walking.root_angle += rotation_dir * angleSpeed;
	} else {
		effect_walking.root_angle = angle; // Align directly if close enough
	}

	
	forward_direction = vec3(sin(effect_walking.root_angle), 0.0f, cos(effect_walking.root_angle));
	vec3 delta_position = forward_direction * speed;
	
	delta_position = collide_with_player(player_position, delta_position);
	
	delta_position = collide_with_zombies(other_zombies, delta_position);	

	rotation_transform r = rotation_axis_angle(vec3(0.0f, 1.0f, 0.0f), effect_walking.root_angle);
	quaternion q = normalize(r.get_quaternion());
	mat3 r_mat = rotation_transform::convert_quaternion_to_matrix(q);
	
	effect_walking.root_position +=delta_position;
	position = effect_walking.root_position;
	character.animated_model.skeleton.joint_matrix_global[0].set_block_translation((effect_walking.root_position));
	character.animated_model.skeleton.joint_matrix_global[0].set_block_linear(r_mat );

	for (unsigned int i = 1; i <  character.animated_model.skeleton.joint_matrix_local.size(); i++)
	{
		int parent_index = character.animated_model.skeleton.parent_index[i];
		character.animated_model.skeleton.joint_matrix_global[i] = character.animated_model.skeleton.joint_matrix_global[parent_index] * character.animated_model.skeleton.joint_matrix_local[i];
	}

	character.animated_model.skeleton.update_joint_matrix_global_to_local();

}

vec3 zombie::collide_with_player(vec3 player_position, vec3 walking_position){
	vec3 d_AB = player_position - position;

	if (norm(d_AB) > 1.7){
		return walking_position;
	}

	return vec3{0,0,0};

	// vec3 forward_direction = vec3(sin(effect_walking.root_angle), 0.0f, cos(effect_walking.root_angle));

	// decompose in perpendicular direction

	// vec3 proj = dot(forward_direction, d_AB)* forward_direction;

}

vec3 zombie::restrict_movement(vec3 other_zombie_pos, vec3 moving_direction){
    vec3 v_ab = utils::remove_y_direction(other_zombie_pos - position);

    float cos_angle = dot(v_ab, moving_direction);

    if (cos_angle < 0) // moving in the opposite direction
        return moving_direction;

    vec3 projection = cos_angle* v_ab;

    return moving_direction - projection;
}

vec3 zombie::collide_with_zombies(std::unordered_map<std::string, zombie> & all_zombies, vec3 walking_direction){
	for (auto [_name, _zombie] : all_zombies){
		if (_name.compare(name) ==0)
			continue;

		vec3 d_AB = _zombie.position - position;

		if (norm(d_AB) < 1.7){
			walking_direction = restrict_movement(_zombie.position, walking_direction);
		}
	}
	return walking_direction;
}