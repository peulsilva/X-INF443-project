#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "camera.hpp"
#include "player.hpp"
#include "zombie.hpp"
#include "minimap.hpp"

#include "animated_character/animated_character.hpp"
#include "effects/effects.hpp"


using cgp::mesh_drawable;




struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = false;
	ImVec2 weapon_window_size = {300, 200};
	ImVec2 helper_window_size = {50,50};
};

// The structure of the custom scene
struct game : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	timer_fps *fps_counter;

	std::vector<mesh_drawable> shapes;

	// Special camera mode adapted to a 2D displacement mode
	camera_controller camera_control;

	std::unordered_map<std::string, zombie> zombies;

	std::unordered_map<weapon_type, weapon> base_weapons;
	zombie base_zombie;

	bool has_dead_zombie; 
	
	std::string current_active_zombie;

	std::vector<std::pair<weapon, vec3>> weapons;
	std::vector<vec3> weapon_pos;

	minimap mini_map;

	std::map<std::string, effect_transition_structure> effect_transition;	
	effect_walking_structure effect_walk;


	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	int zombies_count = 0;

	mesh_drawable ground;
	mesh_drawable cylinder;
	skybox_drawable skybox;

	player this_player;

	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop
	void animate_characters();


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

	void spawn_zombies();

	void spawn_weapons();

	void display_info();

};





