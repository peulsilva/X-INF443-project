#include "scene.hpp"


using namespace cgp;




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
	
	// skybox.texture.initialize_cubemap_on_gpu(
	// 	image_grid[1], //left face
	// 	image_grid[7], // right face
	// 	image_grid[4], // back face
	// 	image_grid[10], // front face
	// 	image_grid[3], // bottom face
	// 	image_grid[5] // top face
	// );


	// terrain

	ground.initialize_data_on_gpu(mesh_primitive_quadrangle({ -1.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, -1.0f }));
ground.model.scaling = 50.0f;
ground.model.translation = { 0.0f, 0.0f, 0.0f }; // No translation needed for y-axis up
ground.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/checkboard.png");

// Define cylinder geometry with y-axis up
cylinder.initialize_data_on_gpu(mesh_primitive_cylinder(0.05f, { 0, -1.0f, 0 }, { 0, 1.0f, 0 }));

	gui.display_frame = true;



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
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{

	this_player.move();
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	this_player.move();
	camera_control.idle_frame(environment.camera_view);
}

