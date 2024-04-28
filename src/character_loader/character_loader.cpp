#include "character_loader.hpp"
#include "../environment.hpp"

using namespace cgp;



character_structure load_character_zombie() {
	filename_loader_structure loader_param;
	loader_param.set_skeleton(project::path+"assets/zombie/skeleton/");
	loader_param.add_rigged_mesh("body", project::path+"assets/zombie/mesh-yaku_zombie/", project::path+"assets/zombie/textures/Yakuzombie_diffuse.png");
	loader_param.add_animation("Walk", project::path+"assets/zombie/animation/");
	
	character_structure character;
	character.load_and_initialize(loader_param, affine_rts().set_scaling(8e-3));

	return character;
}
