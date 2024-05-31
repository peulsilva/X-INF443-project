#include "character_loader.hpp"
#include "../environment.hpp"

using namespace cgp;



character_structure load_character_zombie(int zombie_id) {
	filename_loader_structure loader_param;
	std::cout << zombie_id << std::endl;
	switch (zombie_id){
	case 1:
		loader_param.set_skeleton(project::path+"assets/zombie/1/skeleton/");
		loader_param.add_rigged_mesh("body", project::path+"assets/zombie/1/mesh-yaku_zombie/", project::path+"assets/zombie/1/textures/Yakuzombie_diffuse.png");
		loader_param.add_animation("Walk", project::path+"assets/zombie/1/animation/walk/");
		loader_param.add_animation("Run", project::path+"assets/zombie/1/animation/run/");
		loader_param.add_animation("Death", project::path+"assets/zombie/1/animation/death/");
		loader_param.add_animation("Hit", project::path+"assets/zombie/1/animation/hit/");
		break;

	case 2: 
		loader_param.set_skeleton(project::path+"assets/zombie/2/skeleton/");
		loader_param.add_rigged_mesh("body", project::path+"assets/zombie/2/mesh-survivor/", project::path+"assets/zombie/2/textures/Survivor_body_diffuse.png");
		loader_param.add_animation("Walk", project::path+"assets/zombie/2/animation/walk/");
		break;
	
	default:
		break;
	}
	
	
	character_structure character;
	switch (zombie_id){
	case 1:
		character.load_and_initialize(loader_param, affine_rts().set_scaling(8e-3));
		break;

	case 2:
		character.load_and_initialize(loader_param, affine_rts().set_scaling(8e-3));
		break;
	
	default:
		break;
	}
		
	

	return character;
}
