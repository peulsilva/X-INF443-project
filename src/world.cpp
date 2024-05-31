#include "world.hpp"
#include "constants.hpp"


void world::initialize(std::unordered_map<std::string, vec3>& obstacle_positions){
    ground.initialize_data_on_gpu(mesh_primitive_quadrangle(
		vec3{ -1.0f, 0.0f, -1.0f }, 
		vec3{ -1.0f, 0.0f, 1.0f }, 
		vec3{ 1.0f, 0.0f, 1.0f }, 
		vec3{ 1.0f, 0.0f, -1.0f }
	));
	ground.model.translation = { 0.0f, 0.0f, 0.0f }; // No translation needed for y-axis up
	ground.texture.load_and_initialize_texture_2d_on_gpu(
		project::path + "assets/grass.jpg",
		GL_REPEAT,
		GL_REPEAT
	);
	ground.model.scaling = constants::WORLD_SIZE;

    // house.initialize_data_on_gpu(
    //     mesh_load_file_obj("assets/world/old house.obj")
    // );
    // house.model.rotation = rotation_transform::from_frame_transform(
    //     {0,1,0},
    //     {0,0,1},
    //     {0,0,1},
    //     {0,1,0}
    // );
    // house_position = obstacle_positions["house"];
    
    // house.texture.load_and_initialize_texture_2d_on_gpu("assets/world/SAN_RUMAHGEN05.png");
    // house.model.translation = house_position;
}

void world::draw(const environment_structure& env, bool display_wireframe){
    if (display_wireframe){
        cgp::draw_wireframe(ground,env);
        cgp::draw_wireframe(house, env);
    }
        
    else{
        cgp::draw(ground, env);
        cgp::draw(house, env);    
    }
}

