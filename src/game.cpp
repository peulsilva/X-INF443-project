#include "game.hpp"
#include "audio_controller.hpp"
#include "constants.hpp"

using namespace cgp;

#include "character_loader/character_loader.hpp"

game::game(){}
game::game(const game & other){}

void game::initialize()
{
	// starting variables

	weapons = {};
	weapon_pos = {};
	medicines = {};
	medicine_positions = {};
	has_dead_zombie = false;
	zombies = {};
	


	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();

	environment.uniform_generic.uniform_vec3["fog_color"] = constants::FOG_COLOR;
	environment.background_color = constants::FOG_COLOR;
	environment.uniform_generic.uniform_int["fog_depth"] = constants::FOG_DEPTH;
	display_info();

	// player
	
	this_player = player(vec3{0,0,0,}, camera_control, zombies, obstacles);

	mini_map = minimap();
	std::cout << "initialized minimap " << std::endl;
	game_world.initialize(obstacles);

	global_frame.initialize_data_on_gpu(mesh_primitive_frame());
	global_frame.model.translation = obstacles["house"]; //- vec3{3.5,0,7.2};

	// skybox


	// auto struct_shape = mesh_load_file_obj_advanced(project::path + "assets/worlds/holodeck/", "holodeck.obj");
	// // auto struct_shape = mesh_load_file_obj_advanced(project::path + "assets/sponza/", "sponza.obj");
	
	// shapes = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_shape);


	gui.display_frame = true;

	base_zombie = zombie({0,0,0}, "0", false, obstacles);

	base_weapons = {
		{rifle, weapon(rifle)},
		{handgun, weapon(handgun)},
		{shotgun, weapon(shotgun)},
		{smg, weapon(smg)}
	};

	base_medicine = medicine({0,0,0});
	
	current_active_zombie = "1";

	for(auto& entry : zombies)
		entry.second.character.timer.start();

}

void game::restart(){
	weapons = {};
	weapon_pos = {};
	medicines = {};
	medicine_positions = {};
	zombies = {};
	has_dead_zombie = false;
	game_over = false;
	level = 1;
	
	this_player = player(vec3{0,0,0,}, camera_control, zombies, obstacles);

	mini_map = minimap();

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
	environment.uniform_generic.uniform_vec3["flashlight_direction"] = camera_control.camera_model.front();
	this_player.set_fps(std::max(fps_counter->fps, 20));

	new_level_count++;

	if (constants::SHOULD_SPAWN_ZOMBIES && new_level_count>= new_level_timeout)
		spawn_zombies();
	
	if (constants::SHOULD_SPAWN_WEAPONS)
		spawn_weapons();

	if (constants::SHOULD_SPAWN_MEDICINES)
		spawn_medicine();


	if (is_level_over(level)){
		level++;
		new_level_count = 0;
	}

	if (!this_player.is_alive){
		game_over = true;
	}

	if (gui.display_frame)
		draw(global_frame, environment);

	
	glDisable(GL_DEPTH_TEST) ;

	glEnable(GL_DEPTH_TEST);

	game_world.draw(environment, gui.display_wireframe);

	for (auto& [w, pos] : weapons){
		w.draw_on_scene(environment, pos);
		
	}

	for (int i = 0 ; i < medicines.size(); i++){
		medicines[i].draw(environment, gui.display_wireframe);
	}


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

	std::string health_info = "Health   " +  std::to_string((int)this_player.health);
	health_info += " / 100";
	ImGui::Text(health_info.c_str());

	std::string points = "Points " + std::to_string(this_player.points);
	ImGui::Text(points.c_str());

	std::string level_info = "Round " + std::to_string(level);
	ImGui::Text(level_info.c_str());

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

	ImGui::Begin("medicines in screen", NULL, 
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground
	);
	for (int i = 0; i < medicines.size(); i++){
		vec3 pos = medicine_positions[i];

		if (norm(pos - this_player.position) <2){

			std::string s = "Press F to heal.";
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
	mini_map.add_medicine(medicine_positions);
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

		this_player.heal(medicines, medicine_positions);
	}
	camera_control.action_keyboard(environment.camera_view);
}
void game::idle_frame()
{
	this_player.handle_mouse_click();

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

		if (norm(this_zombie.position - this_player.position) > 100){
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
	// Compute Skinning deformation
	// ********************************** //
	for(auto& [name, zombie] : zombies) {
		if (norm(zombie.position - this_player.position) > constants::MAX_DIST_SHOW_ZOMBIES){
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
		if (norm(zombie.position - this_player.position) > constants::MAX_DIST_SHOW_ZOMBIES){
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

	}
}

void game::spawn_zombies(){

	float prob = std::rand() / ((float) RAND_MAX);
	bool should_spawn = true;

	if (this_player.n_kills + zombies.size() == get_total_zombies_at_end_of_level(level)){
		should_spawn = false;
	}

	if (prob < constants::PROBABILITY_SPAWN_ZOMBIES){
		std::string idx_zombie = std::to_string(zombies_count++);

		int pos_x = std::rand() % (2*constants::WORLD_SIZE) - constants::WORLD_SIZE;
		int pos_z = std::rand() % (2*constants::WORLD_SIZE) -constants::WORLD_SIZE;

		while (norm(this_player.position - vec3{pos_x,0,pos_z} ) < constants::MAX_DIST_SPAWN_ZOMBIES){
			std::cout << "error in initializing zombie" << std::endl;
			
			pos_x = std::rand() % (2*constants::WORLD_SIZE) - constants::WORLD_SIZE;
			pos_z = std::rand() % (2*constants::WORLD_SIZE) -constants::WORLD_SIZE;
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

			

			if (erased_any && should_spawn){
				// zombies[idx_zombie]= zombie(spawn_positions[spawn_position_idx], idx_zombie);	
				zombies[idx_zombie]= base_zombie;
				zombies[idx_zombie].name= idx_zombie;
				zombies[idx_zombie].show = true;
				zombies[idx_zombie].health += level*10;
				zombies[idx_zombie].position += {pos_x, 0, pos_z};	
				zombies[idx_zombie].effect_walking.root_position += {pos_x, 0, pos_z};	
				zombies[idx_zombie].is_running = level >=0;
			}
			
			has_dead_zombie = false;
		}

		else if (zombies.size() < constants::MAX_ZOMBIES_ON_SCREEN && should_spawn){
			zombies[idx_zombie]= base_zombie;
			zombies[idx_zombie].name= idx_zombie;
			zombies[idx_zombie].show = true;
			zombies[idx_zombie].health += level*10;
			zombies[idx_zombie].position += {pos_x, 0, pos_z};	
			zombies[idx_zombie].effect_walking.root_position += {pos_x, 0, pos_z};	

			zombies[idx_zombie].is_running = level >=0 ;
		}
	}
}

void game::spawn_weapons(){

	if (weapons.size() >= constants::MAX_SPAWN_WEAPONS)
		return;

	float prob = std::rand() / ((float) RAND_MAX);

	if (prob < constants::PROBABILITY_SPAWN_WEAPONS){

		int pos_x = std::rand() % (2*constants::ACESSIBLE_AREA) - constants::ACESSIBLE_AREA;
		int pos_z = std::rand() % (2*constants::ACESSIBLE_AREA) -constants::ACESSIBLE_AREA;

		weapon_type random_weapon = weapon::choose_random_weapon();

		weapon this_weapon = base_weapons[random_weapon];
		weapons.push_back({this_weapon, {pos_x, 0, pos_z}});

		weapon_pos.push_back({pos_x, 0, pos_z});

	}
}

void game::spawn_medicine(){
	if (medicines.size() >= constants::MAX_SPAWN_MEDICINE){
		return;
	}

	float prob = std::rand() / ((float) RAND_MAX);

	if (prob < constants::PROBABILITY_SPAWN_MEDICINES){

		int pos_x = std::rand() % (2*constants::ACESSIBLE_AREA) - constants::ACESSIBLE_AREA;
		int pos_z = std::rand() % (2*constants::ACESSIBLE_AREA) -constants::ACESSIBLE_AREA;
	

		medicine this_medicine = base_medicine;
		this_medicine.position += {pos_x, 0, pos_z};

		medicines.push_back(this_medicine);

		medicine_positions.push_back({pos_x, 0, pos_z});

	}
}

int game::get_num_zombies_for_level(int _level){
	return constants::RATIO_ZOMBIES_PER_LEVEL*_level;
}

int game::get_total_zombies_at_end_of_level(int _level){
	return (_level*(constants::RATIO_ZOMBIES_PER_LEVEL + get_num_zombies_for_level(_level)))/2;
}

bool game::is_level_over(int _level){
	return this_player.n_kills == get_total_zombies_at_end_of_level(_level);
}