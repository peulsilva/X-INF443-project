#include "game.hpp"
#include "audio_controller.hpp"
#include "constants.hpp"

using namespace cgp;

#include "character_loader/character_loader.hpp"

void game::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();

	environment.uniform_generic.uniform_vec3["fog_color"] = 0.3*vec3{1,1,1};
	environment.background_color = 0.3*vec3{1,1,1};
	environment.uniform_generic.uniform_int["fog_depth"] = constants::FOG_DEPTH;
	display_info();

	// player
	
	this_player = player(vec3{0,0,0,}, camera_control, zombies);

	mini_map = minimap();

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
	ground.model.scaling = constants::WORLD_SIZE;
	ground.model.translation = { 0.0f, 0.0f, 0.0f }; // No translation needed for y-axis up
	ground.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/grass.jpg");


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

void game::display_info()
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

void game::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	this_player.curr_weapon.set_fps(std::max(fps_counter->fps, 20));

	if (gui.display_frame)
		draw(global_frame, environment);

	
	glDisable(GL_DEPTH_TEST) ;
	
	
    // draw(skybox, environment);

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

void game::display_gui()
{

	ImGui::Begin("weapon", NULL, 
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground
	);
	ImGui::SetWindowSize(gui.weapon_window_size);
	ImGui::SetWindowPos({
		window.screen_resolution_width - gui.weapon_window_size.x, 
		window.screen_resolution_height - gui.weapon_window_size.y
	});

	ImGui::SetWindowFontScale(2);
	ImGui::Text("----------------------------------------------------");
	std::string weapon_info = this_player.curr_weapon.get_weapon_name();
	weapon_info += "   ";
	weapon_info += std::to_string(this_player.curr_weapon.bullets_in_clip);
	weapon_info += " / ";
	weapon_info += std::to_string(this_player.curr_weapon.total_bullets);
	ImGui::Text(weapon_info.c_str());
	ImGui::Text("----------------------------------------------------");
	ImGui::End();

	ImGui::Begin("weapons in screen", NULL, 
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground
	);
	for (auto& [w, pos]: weapons){
		if (norm(pos - this_player.position)  <2){

			std::string s = "Press F to get " + w.get_weapon_name();
			ImGui::SetWindowSize(ImVec2{200, 10});
			ImGui::Text(s.c_str());
			ImGui::SetWindowPos({
				window.screen_resolution_width/2., 
				window.screen_resolution_height/2.
			});
		}
	}
	ImGui::End();


	ImGui::Begin("minimap", NULL, 
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground
	);

	mini_map.add_weapons(weapon_pos);
	mini_map.update(this_player.position.x, this_player.position.z);

	ImGui::SetWindowSize(ImVec2{400, 800});
	ImGui::SetWindowPos({
		0, 
		window.screen_resolution_height*2/3.
	});
	// ImGui::SetWindowFontScale(0.5);

	mini_map.print();
	
	ImGui::End();
}

void game::mouse_move_event()
{
	
	camera_control.action_mouse_move(environment.camera_view);
}
void game::mouse_click_event()
{
	this_player.handle_mouse_click();
	camera_control.action_mouse_click(environment.camera_view);
}
void game::keyboard_event()
{

	this_player.move();
	auto input = camera_control.inputs;
	if (input->keyboard.is_pressed(GLFW_KEY_F)){
		this_player.get_weapon(weapons, weapon_pos);
	}
	camera_control.action_keyboard(environment.camera_view);
}
void game::idle_frame()
{
	this_player.handle_mouse_click();
	// spawn_zombies();
	spawn_weapons();
	this_player.move();
	camera_control.idle_frame(environment.camera_view);
}

void game::animate_characters(){
	for(auto& [name, zombie]: zombies) {
		zombie.character.timer.update();
	}

	// ************************************************* //
	// Update the current skeleton of each character
	// ************************************************* //
	for(auto& [name, this_zombie] : zombies) {
		character_structure& character = this_zombie.character;
		effect_transition_structure& transition = effect_transition[name];

		if (norm(this_zombie.position - this_player.position) > constants::MAX_DIST_SPAWN_ZOMBIES){
			this_zombie.move(this_player.position, zombies);
			continue;
		}

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
		if (norm(zombie.position - this_player.position) > constants::MAX_DIST_SPAWN_ZOMBIES){
			continue;
		}
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
		if (norm(zombie.position - this_player.position) > constants::MAX_DIST_SPAWN_ZOMBIES){
			continue;
		}
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

void game::spawn_zombies(){

	float prob = std::rand() / ((float) RAND_MAX);



	if (prob < constants::PROBABILITY_SPAWN_ZOMBIES){
		std::string idx_zombie = std::to_string(zombies_count++);

		int pos_x = std::rand() % (2*constants::WORLD_SIZE) - constants::WORLD_SIZE;
		int pos_z = std::rand() % (2*constants::WORLD_SIZE) -constants::WORLD_SIZE;

		while (norm(this_player.position - vec3{pos_x,0,pos_z} ) < constants::MAX_DIST_SPAWN_ZOMBIES){
			int pos_x = std::rand() % (2*constants::WORLD_SIZE) - constants::WORLD_SIZE;
			int pos_z = std::rand() % (2*constants::WORLD_SIZE) -constants::WORLD_SIZE;
		}

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
				zombies[idx_zombie].position += {pos_x, 0, pos_z};	
				zombies[idx_zombie].effect_walking.root_position += {pos_x, 0, pos_z};	
				zombies[idx_zombie].is_running = true;
			}
			
			has_dead_zombie = false;
		}

		else if (zombies.size() < constants::MAX_ZOMBIES_ON_SCREEN){
			zombies[idx_zombie]= base_zombie;
			zombies[idx_zombie].name= idx_zombie;
			zombies[idx_zombie].show = true;
			zombies[idx_zombie].position += {pos_x, 0, pos_z};	
			zombies[idx_zombie].effect_walking.root_position += {pos_x, 0, pos_z};	

			zombies[idx_zombie].is_running = true;
		}
	}
}

void game::spawn_weapons(){

	if (weapons.size() >= constants::MAX_SPAWN_WEAPONS)
		return;

	float prob = std::rand() / ((float) RAND_MAX);

	if (prob < constants::PROBABILITY_SPAWN_WEAPONS){

		int pos_x = std::rand() % (2*constants::WORLD_SIZE) - constants::WORLD_SIZE;
		int pos_z = std::rand() % (2*constants::WORLD_SIZE) -constants::WORLD_SIZE;

		weapon_type random_weapon = weapon::choose_random_weapon();

		weapon this_weapon = base_weapons[random_weapon];
		weapons.push_back({this_weapon, {pos_x, 0, pos_z}});

		weapon_pos.push_back({pos_x, 0, pos_z});

	}
}