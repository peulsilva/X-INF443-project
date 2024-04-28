#include "zombie.hpp"
using namespace cgp;

zombie::zombie(vec3 _position){
    position = _position;

    character = load_character_zombie();
    effect_walking.root_position = position + vec3{0,1,0};
	
}

zombie::zombie(const zombie& other) {
    position = other.position;
    character = other.character; // Assuming character_structure supports copy construction
    // character.animated_model.apply_transformation(position);

}


zombie::zombie(){}

void zombie::walk(
	vec3 player_position
)
{   
    
	// Calculate direction from zombie to player
	vec3 forward_direction = vec3(sin(effect_walking.root_angle), 0.0f, cos(effect_walking.root_angle));
	vec3 zombie_to_player = (player_position - position) - forward_direction;

	// Normalize the direction vector
	vec3 zombie_to_player_normalized = utils::remove_y_direction(zombie_to_player);

	// Calculate the angle between the forward direction of the zombie and the direction to the player
	float angle = acos(zombie_to_player_normalized.z);

	// std::cout << angle << std::endl;
	if (zombie_to_player_normalized.x < 0)
		angle *= -1; 

	// Adjust the angle gradually to smoothly turn towards the player
	float angleSpeed = 0.1f;

	if (abs(angle - effect_walking.root_angle) > angleSpeed) {
		float rotation_dir = angle - effect_walking.root_angle > 0.0f ? 1.0f : -1.0f;
		effect_walking.root_angle += rotation_dir * angleSpeed;
	} else {
		effect_walking.root_angle = angle; // Align directly if close enough
	}

	
	forward_direction = vec3(sin(effect_walking.root_angle), 0.0f, cos(effect_walking.root_angle));
	vec3 delta_position = forward_direction * speed;
	effect_walking.root_position += delta_position;
	

	
	rotation_transform r = rotation_axis_angle(vec3(0.0f, 1.0f, 0.0f), effect_walking.root_angle);
	quaternion q = normalize(r.get_quaternion());
	mat3 r_mat = rotation_transform::convert_quaternion_to_matrix(q);
	
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