#include "scene.hpp"
#include "audio_controller.hpp"
#include "constants.hpp"

using namespace cgp;

#include "character_loader/character_loader.hpp"

void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();
	display_info();

	// player
	
	this_player = player(vec3{0,0,0,}, camera_control, zombies);


	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// skybox

	image_structure image_skybox = image_load_file("assets/skybox.jpg");

	std::vector<image_structure> image_grid = image_split_grid(image_skybox, 4, 3);

	skybox.initialize_data_on_gpu();
	skybox.texture.initialize_cubemap_on_gpu(image_grid[1], image_grid[7], image_grid[5], image_grid[3], image_grid[10], image_grid[4]);


	// auto struct_shape = mesh_load_file_obj_advanced(project::path + "assets/worlds/holodeck/", "holodeck.obj");
	// // auto struct_shape = mesh_load_file_obj_advanced(project::path + "assets/sponza/", "sponza.obj");
	
	// shapes = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_shape);


	// terrain

	ground.initialize_data_on_gpu(mesh_primitive_quadrangle({ -1.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, -1.0f }));
	ground.model.scaling = 50.0f;
	ground.model.translation = { 0.0f, 0.0f, 0.0f }; // No translation needed for y-axis up
	ground.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/checkboard.png");


	gui.display_frame = true;

	base_zombie = zombie({0,0,0}, "0", false);

	base_weapons = {
		{rifle, weapon(rifle)},
		{handgun, weapon(handgun)},
		{shotgun, weapon(shotgun)},
		{smg, weapon(smg)}
	};
	
	current_active_zombie = "1";

	for(auto& entry : zombies)
		entry.second.character.timer.start();

}

void scene_structure::display_info()
{
	std::cout << "\nCAMERA CONTROL:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << camera_control.doc_usage() << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;


	std::cout << "\nSCENE INFO:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "This scene allows the character to give the impression of being able to walk on the ground using the mouse and keyboard (keys or WSAD/ZSQD)." << std::endl;
	std::cout << "For game-like mode: Use 'Shift+F' for full screen; 'Shift+C' for mouse capture." << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl ;
}

void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	
	glDisable(GL_DEPTH_TEST) ;
	
	
    draw(skybox, environment);

	glEnable(GL_DEPTH_TEST);

	

	draw(ground, environment);

	for (auto& [w, pos] : weapons){
		w.draw_on_scene(environment, pos);
	}

	if (gui.display_wireframe)
		draw_wireframe(ground, environment);

	animate_characters();

    glClear(GL_DEPTH_BUFFER_BIT);
	this_player.draw(environment, gui.display_wireframe);
	
	
}

void scene_structure::display_gui()
{
	// ImGui::Checkbox("Frame", &gui.display_frame);
	// ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}

void scene_structure::mouse_move_event()
{
	
	camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	this_player.handle_mouse_click();
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{

	this_player.move();
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	this_player.handle_mouse_click();
	spawn_zombies();
	spawn_weapons();
	this_player.move();
	camera_control.idle_frame(environment.camera_view);
}

void scene_structure::animate_characters(){
	for(auto& [name, zombie]: zombies) {
		zombie.character.timer.update();
	}

	// ************************************************* //
	// Update the current skeleton of each character
	// ************************************************* //
	for(auto& [name, this_zombie] : zombies) {
		character_structure& character = this_zombie.character;
		effect_transition_structure& transition = effect_transition[name];

		if (!this_zombie.is_alive)
			has_dead_zombie = true;

		// Default animation reading a standard animation cycle
		bool is_visible = dot(this_zombie.position, this_player.position) > 0;
		if(transition.active==false) {
			if (this_zombie.is_alive || this_zombie.display_death_animation){
				character.animated_model.set_skeleton_from_animation(character.current_animation_name, character.timer.t_periodic);
				
			}
			else{
				continue;
			}
		}
		// Currently with an active transition between two animations
		else {
			effect_transition_compute(transition, character);
			effect_transition_stop_if_completed(transition, character);
		}
		this_zombie.move(this_player.position, zombies);
	}

	// ********************************** //
	// Apply effects on the skeleton
	// ********************************** //

	// Apply the walk effect if activated

	// zombies[current_active_zombie].character.animated_model.apply_transformation({0,1,0});
	// Apply the head rotation effect if activated
	// if(gui.rotate_head_effect_active) {
	// 	for(auto& entry_character : characters) {
	// 		effect_rotate_head_toward_objective_position(entry_character.second.animated_model.skeleton, 10, camera_control.camera_model.position());
	// 	}
	// }


	// ********************************** //
	// Compute Skinning deformation
	// ********************************** //
	for(auto& [name, zombie] : zombies) {
		animated_model_structure& animated_model = zombie.character.animated_model;
		for(auto& rigged_mesh_entry : animated_model.rigged_mesh) {
			std::string mesh_name = rigged_mesh_entry.first;
			animated_model.skinning_lbs(mesh_name);
		}
	}

	// ************************************** //
	// Display the surface and the skeletons
	// ************************************** //
	for(auto& [name, zombie] : zombies) {
		character_structure& character = zombie.character;
		animated_model_structure& animated_model = zombie.character.animated_model;

		// Display meshes
		for(auto& rigged_mesh_entry : animated_model.rigged_mesh) {
			std::string mesh_name = rigged_mesh_entry.first;
			rigged_mesh_structure& rigged_mesh = rigged_mesh_entry.second;
			
			mesh_drawable& drawable = character.drawable[mesh_name];
			drawable.vbo_position.update(rigged_mesh.mesh_deformed.position);
			drawable.vbo_normal.update(rigged_mesh.mesh_deformed.normal);

			// if(gui.display_surface) {
			draw(drawable, environment);
			// }
			if(gui.display_wireframe) {
				draw_wireframe(drawable, environment);
			}
		}

		// Display skeleton
		// if(gui.display_skeleton) {
		// 	character.sk_drawable.update(animated_model.skeleton);
		// 	character.sk_drawable.display_joint_frame = gui.display_skeleton_joint_frame;
		// 	character.sk_drawable.display_joint_sphere = gui.display_skeleton_joint_sphere;
		// 	character.sk_drawable.display_segments = gui.display_skeleton_bone;
		// 	draw(character.sk_drawable, environment);
		// }

	}
}

void scene_structure::spawn_zombies(){
	std::vector<vec3> spawn_positions = {
		vec3(12, 0, 28),
		vec3(-35, 0, -10),
		vec3(40, 0, -20),
		vec3(25, 0, 45),
		vec3(-8, 0, 30),
		vec3(3, 0, -40),
		vec3(-45, 0, 20),
		vec3(-20, 0, 38),
		vec3(17, 0, -15),
		vec3(-30, 0, -45)
	};

	float prob = std::rand() / ((float) RAND_MAX);



	if (prob < 1/200.){
		std::string idx_zombie = std::to_string(zombies_count++);

		int spawn_position_idx = std::rand() % spawn_positions.size();

		if (zombies.size() >= constants::MAX_ZOMBIES_ON_SCREEN && has_dead_zombie){
			bool erased_any = false;

			std::vector<std::string> should_erase;

			for (auto& [name, _zombie] : zombies){
				if (!_zombie.is_alive){
					// zombies.erase(name);
					should_erase.push_back(name);
					erased_any = true;
				}
			}

			for (auto & n : should_erase){
				zombies.erase(n);
			}

			if (erased_any){
				// zombies[idx_zombie]= zombie(spawn_positions[spawn_position_idx], idx_zombie);	
				zombies[idx_zombie]= base_zombie;
				zombies[idx_zombie].name= idx_zombie;
				zombies[idx_zombie].show = true;
				zombies[idx_zombie].position += spawn_positions[spawn_position_idx];	
				zombies[idx_zombie].effect_walking.root_position += spawn_positions[spawn_position_idx];	
			}
			
			has_dead_zombie = false;
		}

		else if (zombies.size() < constants::MAX_ZOMBIES_ON_SCREEN){
			zombies[idx_zombie]= base_zombie;
			zombies[idx_zombie].name= idx_zombie;
			zombies[idx_zombie].show = true;
			zombies[idx_zombie].position += spawn_positions[spawn_position_idx];	
			zombies[idx_zombie].effect_walking.root_position += spawn_positions[spawn_position_idx];	
		}
	}
}

void scene_structure::spawn_weapons(){

	
	float prob = std::rand() / ((float) RAND_MAX);

	if (prob < 1/2000.){

		int pos_x = std::rand() % 50;
		int pos_z = std::rand() % 50;

		weapon_type random_weapon = weapon::choose_random_weapon();

		weapon this_weapon = base_weapons[random_weapon];
		weapons.push_back({this_weapon, {pos_x, 0, pos_z}});

	}
}