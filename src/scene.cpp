#include "scene.hpp"

using namespace cgp;

#include "character_loader/character_loader.hpp"

void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_y();
	display_info();

	// player
	
	this_player = player(vec3{0,0,0,}, camera_control);

	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// skybox

	image_structure image_skybox = image_load_file("assets/skybox_02.jpg");

	std::vector<image_structure> image_grid = image_split_grid(image_skybox, 4, 3);

	skybox.initialize_data_on_gpu();
	skybox.texture.initialize_cubemap_on_gpu(image_grid[1], image_grid[7], image_grid[5], image_grid[3], image_grid[10], image_grid[4]);



	// terrain

	ground.initialize_data_on_gpu(mesh_primitive_quadrangle({ -1.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, -1.0f }));
	ground.model.scaling = 50.0f;
	ground.model.translation = { 0.0f, 0.0f, 0.0f }; // No translation needed for y-axis up
	ground.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/checkboard.png");

	// Define cylinder geometry with y-axis up
	cylinder.initialize_data_on_gpu(mesh_primitive_cylinder(0.05f, { 0, -1.0f, 0 }, { 0, 1.0f, 0 }));

		gui.display_frame = true;


	// zombie
	zombies["1"] = zombie(vec3{20,0,0});
	zombies["2"] = zombie(vec3{-20,0,0});
	// characters["1"].set_current_animation("walk");
	
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

	for (int kx = -20; kx < 20; kx += 2) {
		for (int kz = -20; kz < 20; kz += 2) {
			if (kx != 0 || kz != 0) {
				// Adjust translation vector to have y-coordinate
				cylinder.model.translation = { kx, 0.0f, kz };
				draw(cylinder, environment);
				if (gui.display_wireframe)
					draw_wireframe(cylinder, environment);
			}
		}
	}

	draw(ground, environment);

	if (gui.display_wireframe)
		draw_wireframe(ground, environment);

	animate_characters();

    glClear(GL_DEPTH_BUFFER_BIT);
	this_player.draw(environment, gui.display_wireframe);
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
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

		// Default animation reading a standard animation cycle
		if(transition.active==false) {
			character.animated_model.set_skeleton_from_animation(character.current_animation_name, character.timer.t_periodic);
		}
		// Currently with an active transition between two animations
		else {
			effect_transition_compute(transition, character);
			effect_transition_stop_if_completed(transition, character);
		}
		this_zombie.walk(this_player.position);
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