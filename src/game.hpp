#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "camera.hpp"
#include "player.hpp"
#include "zombie.hpp"
#include "minimap.hpp"
#include "medicine.hpp"
#include "animated_character/animated_character.hpp"
#include "effects/effects.hpp"
#include "world.hpp"

using cgp::mesh_drawable;




struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = false;
	ImVec2 weapon_window_size = {300, 300};
	ImVec2 helper_window_size = {50,50};
};

// The structure of the custom scene
class game : cgp::scene_inputs_generic {
	public:
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

	bool game_over = false;

	// Special camera mode adapted to a 2D displacement mode
	camera_controller camera_control;

	std::unordered_map<std::string, zombie> zombies;
	std::unordered_map<std::string, vec3> obstacles = {
		{"tree1", {-53, 0, -36}},
		{"tree2", {95, 0, 20}},
		{"tree3", {50, 0, -62}},
		{"tree4", {40, 0, -17}},
		{"tree5", {-26, 0, 4}},
		{"tree6", {-61, 0, 60}},
		{"tree7", {-54, 0, 90}},
		{"tree8", {67, 0, -2}},
		{"tree9", {-89, 0, -98}},
		{"tree10", {51, 0, -49}},
		{"tree11", {51, 0, 58}},
		{"tree12", {-54, 0, -90}},
		{"tree13", {63, 0, 90}},
		{"tree14", {68, 0, -50}},
		{"tree15", {40, 0, -46}},
		{"tree16", {76, 0, -59}},
		{"tree17", {77, 0, -52}},
		{"tree18", {3, 0, -95}},
		{"tree19", {22, 0, -98}},
		{"tree20", {-38, 0, 10}},
		{"tree21", {-49, 0, -77}},
		{"tree22", {-29, 0, -41}},
		{"tree23", {27, 0, 63}},
		{"tree24", {-86, 0, -80}},
		{"tree25", {15, 0, -20}},
		{"tree26", {-100, 0, -44}},
		{"tree27", {-5, 0, -60}},
		{"tree28", {38, 0, 29}},
		{"tree29", {-74, 0, 15}},
		{"tree30", {42, 0, -76}},
		{"rock1", {-1, 0, 0}},
		// {"rock2", {-39, 0, -25}},
		// {"rock3", {-9, 0, 43}},
		// {"rock4", {-17, 0, 74}},
		// {"rock5", {-20, 0, -76}},
		// {"rock6", {-12, 0, 62}},
		// {"rock7", {-82, 0, -63}},
		// {"rock8", {75, 0, 13}},
		// {"rock9", {-88, 0, -68}},
		// {"rock10", {7, 0, -27}},
		// {"rock11", {21, 0, 86}},
		// {"rock12", {40, 0, 5}},
		// {"rock13", {-21, 0, 78}},
		// {"rock14", {-45, 0, 84}},
		// {"rock15", {47, 0, -21}},
		// {"rock16", {-33, 0, -31}},
		// {"rock17", {83, 0, 42}},
		// {"rock18", {-37, 0, 89}},
		// {"rock19", {35, 0, 99}},
		// {"rock20", {57, 0, 60}},
		// {"rock21", {-14, 0, -9}},
		// {"rock22", {21, 0, -20}},
		// {"rock23", {-86, 0, -42}},
		// {"rock24", {-16, 0, 18}},
		// {"rock25", {14, 0, -77}},
		// {"rock26", {-72, 0, 94}},
		// {"rock27", {18, 0, 51}},
		// {"rock28", {-84, 0, 53}},
		// {"rock29", {48, 0, -16}},
		// {"rock30", {-3, 0, 75}},
		// {"rock31", {31, 0, 18}},
		// {"rock32", {-28, 0, -53}},
		// {"rock33", {-1, 0, 60}},
		// {"rock34", {-22, 0, 36}},
		// {"rock35", {36, 0, 25}},
		// {"rock36", {1, 0, -58}},
		// {"rock37", {-38, 0, 56}},
		// {"rock38", {4, 0, -12}},
		// {"rock39", {64, 0, 83}},
		// {"rock40", {-91, 0, 69}},
		// {"rock41", {77, 0, -64}},
		// {"rock42", {91, 0, -28}},
		// {"rock43", {70, 0, 71}},
		// {"rock44", {34, 0, 15}},
		// {"rock45", {-36, 0, 13}},
		// {"rock46", {-92, 0, 58}},
		// {"rock47", {15, 0, -97}},
		// {"rock48", {-34, 0, -72}},
		// {"rock49", {-51, 0, -66}},
		// {"rock50", {-47, 0, 19}},
		// {"rock51", {73, 0, 85}},
		// {"rock52", {81, 0, -61}},
		// {"rock53", {67, 0, 1}},
		// {"rock54", {-16, 0, -19}},
		// {"rock55", {38, 0, -40}},
		// {"rock56", {61, 0, 70}},
		// {"rock57", {-6, 0, 10}},
		// {"rock58", {21, 0, -13}},
		// {"rock59", {66, 0, -88}},
    	// {"rock60", {-80, 0, -18}}
	};



	std::unordered_map<weapon_type, weapon> base_weapons;
	zombie base_zombie;

	medicine base_medicine;

	std::vector<medicine> medicines;
	std::vector<vec3> medicine_positions;

	bool has_dead_zombie; 

	int level = 1;

	// zombies will not spawn until new_level_count >= new_level_timeout
	int new_level_count = 0;
	int new_level_timeout = 100;
	
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

	world game_world;
	mesh_drawable cylinder;
	skybox_drawable skybox;

	player this_player;

	// ****************************** //
	// Functions
	// ****************************** //


	void initialize();    // Standard initialization to be called before the animation loop
	void restart();
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop
	void animate_characters();

	game(const game& other);
	game();

	int get_num_zombies_for_level(int _level);
	int get_total_zombies_at_end_of_level(int _level);
	bool is_level_over(int _level);

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

	void spawn_zombies();
	void spawn_weapons();
	void spawn_medicine();

	void display_info();
	

};







