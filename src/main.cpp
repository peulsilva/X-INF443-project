

#include "cgp/cgp.hpp" // Give access to the complete CGP library
#include "environment.hpp" // The general game_ environment + project variable
#include <iostream> 
#include "audio_controller.hpp"

#include <chrono>
#include <thread>

// Custom game_ of this code
#include "game.hpp"




// *************************** //
// Custom game_ defined in "game_.hpp"
// *************************** //

game game_;

window_structure standard_window_initialization();
void initialize_default_shaders();
void animation_loop();
void display_gui_default();

bool render_menu = true;
bool game_over = false;
bool reset_game = false;
int menu_timer = 0;
int restart_game_timer = 0;
int restart_game_timeout = 50;
int menu_timeout = 50;

timer_fps fps_record;

void main_menu(){

	emscripten_update_window_size(game_.window.width, game_.window.height); // update window size in case of use of emscripten (not used by default)
    glfwSetInputMode(game_.window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	game_.camera_projection.aspect_ratio = game_.window.aspect_ratio();
	game_.environment.camera_projection = game_.camera_projection.matrix();
	glViewport(0, 0, game_.window.width, game_.window.height);

	vec3 const& background_color = 0.*vec3{1,1,1};
	glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	float const time_interval = fps_record.update();
	glfwSetWindowTitle(game_.window.glfw_window, "X-zombies");

	imgui_create_frame();
	ImGui::GetIO().FontGlobalScale = project::gui_scale;
	game_.inputs.mouse.on_gui = ImGui::GetIO().WantCaptureMouse;
	game_.inputs.time_interval = time_interval;

	ImVec2 window_size(500, 600);
	ImGui::Begin("X-zombies", NULL, 
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground
	);
	ImGui::Text("		 --------------------------");
	ImGui::Text("		 | Welcome to X-zombies |");
	ImGui::Text("		 --------------------------");
	ImGui::Text(" ");
	ImGui::Text("To move: A W S D");
	ImGui::Text(" ");
	ImGui::Text("To shoot: Right mouse button");
	ImGui::Text("To aim: Left mouse button");
	ImGui::Text("To reload your weppon: R");
	ImGui::Text(" ");
	ImGui::Text("To access the menu: P");
	ImGui::Text("If you are out of ammo, you can look for other weapons in the map");
	ImGui::Text(" ");
	ImGui::Text(" ");
	ImGui::Text("Good luck and have fun!");
	ImGui::Text(" ");
	ImGui::Text(" ");



	ImGui::SetWindowSize(window_size);
	ImGui::SetWindowPos({
		(float)(game_.window.width - window_size.x)/2.0f,
		(float)(game_.window.height - window_size.y)/2.0f,
	});
	ImGui::End();

	// End of ImGui display and handle GLFW events
	imgui_render_frame(game_.window.glfw_window);
	glfwSwapBuffers(game_.window.glfw_window);
	glfwPollEvents();

}

void game_over_screen(){
	emscripten_update_window_size(game_.window.width, game_.window.height); // update window size in case of use of emscripten (not used by default)
    glfwSetInputMode(game_.window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	game_.camera_projection.aspect_ratio = game_.window.aspect_ratio();
	game_.environment.camera_projection = game_.camera_projection.matrix();
	glViewport(0, 0, game_.window.width, game_.window.height);

	vec3 const& background_color = 0.*vec3{1,1,1};
	glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	float const time_interval = fps_record.update();
	glfwSetWindowTitle(game_.window.glfw_window, "X-zombies");

	imgui_create_frame();
	ImGui::GetIO().FontGlobalScale = project::gui_scale;
	game_.inputs.mouse.on_gui = ImGui::GetIO().WantCaptureMouse;
	game_.inputs.time_interval = time_interval;

	ImVec2 window_size(500, 600);
	ImGui::Begin("X-zombies", NULL, 
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground
	);

	ImGui::SetWindowFontScale(2);
	ImGui::Text("--------------------------");
	ImGui::Text("You are dead");
	ImGui::Text("Press any key to restart");
	ImGui::Text("--------------------------");




	ImGui::SetWindowSize(window_size);
	ImGui::SetWindowPos({
		(float)(game_.window.width - window_size.x)/2.0f,
		(float)(game_.window.height - window_size.y)/2.0f,
	});
	ImGui::End();

	// End of ImGui display and handle GLFW events
	imgui_render_frame(game_.window.glfw_window);
	glfwSwapBuffers(game_.window.glfw_window);
	glfwPollEvents();

}

// The rest of this code is a generic initialization and animation loop that can be applied to different game_s
// *************************** //
// Start of the program
// *************************** //

int main(int, char* argv[])
{
	std::cout << "Run " << argv[0] << std::endl;

	

	// ************************ //
	//     INITIALISATION
	// ************************ //
	
	// Standard Initialization of an OpenGL ready window
	game_.window = standard_window_initialization();

	// Initialize default path for assets
	project::path = cgp::project_path_find(argv[0], "shaders/");

	// Initialize default shaders
	initialize_default_shaders();


	// Custom game_ initialization
	std::cout << "Initialize data of the game_ ..." << std::endl;
	game_.initialize();
	std::cout << "Initialization finished\n" << std::endl;

	std::cout << "Start menu loop ..." << std::endl;

	// ************************ //
	//     Animation Loop
	// ************************ //
	std::cout << "Start animation loop ..." << std::endl;
	fps_record.start();
	
	init_sound_effects();

	// Call the main display loop in the function animation_loop
	//  The following part is simply a loop that call the function "animation_loop"
	//  (This call is different when we compile in standard mode with GLFW, than when we compile with emscripten to output the result in a webpage.)
#ifndef __EMSCRIPTEN__
    double lasttime = glfwGetTime();
	// Default mode to run the animation/display loop with GLFW in C++
	
	while (!glfwWindowShouldClose(game_.window.glfw_window)) {
		// The real animation loop

		menu_timer++;	
		game_.fps_counter = &fps_record;

		if (game_.game_over){
			game_over = true;
		}

		if (game_over){
			restart_game_timer++;

			game_over_screen();

			if (reset_game){
				render_menu = true;

				game_.restart();

				reset_game = false;
				game_over = false;
				menu_timer = 0;
			}
		}

		else if (render_menu){
			main_menu();
		}


		else
			animation_loop();
		// FPS limitation
		if(project::fps_limiting){
			while (glfwGetTime() < lasttime + 1.0 / project::fps_max) {	}
        	lasttime = glfwGetTime();
		}
	}
#else
	// Specific loop if compiled for EMScripten
	emscripten_set_main_loop(animation_loop, 0, 1);
#endif

	std::cout << "\nAnimation loop stopped" << std::endl;

	// Cleanup
	cgp::imgui_cleanup();
	glfwDestroyWindow(game_.window.glfw_window);
	glfwTerminate();

	return 0;
}

void animation_loop()
{

	emscripten_update_window_size(game_.window.width, game_.window.height); // update window size in case of use of emscripten (not used by default)

	game_.camera_projection.aspect_ratio = game_.window.aspect_ratio();
	game_.environment.camera_projection = game_.camera_projection.matrix();
	glViewport(0, 0, game_.window.width, game_.window.height);

	vec3 const& background_color = game_.environment.background_color;
	glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	float const time_interval = fps_record.update();
	if (fps_record.event) {
		std::string const title = "X-zombies - " + str(fps_record.fps) + " fps";
		glfwSetWindowTitle(game_.window.glfw_window, title.c_str());
	}

	imgui_create_frame();
	ImGui::GetIO().FontGlobalScale = project::gui_scale;
	// ImGui::Begin("GUI", NULL, ImGuiWindowFlags_NoDecoration);
	
	game_.inputs.mouse.on_gui = ImGui::GetIO().WantCaptureMouse;
	game_.inputs.time_interval = time_interval;


	// Display the ImGUI interface (button, sliders, etc)
	display_gui_default();
	game_.display_gui();

	// Handle camera behavior in standard frame
	game_.idle_frame();

	// Call the display of the game_
	game_.display_frame();


	// End of ImGui display and handle GLFW events
	imgui_render_frame(game_.window.glfw_window);
	glfwSwapBuffers(game_.window.glfw_window);
	glfwPollEvents();
}


void initialize_default_shaders()
{
	// Generate the default directory from which the shaders are found
	//  By default, it should be "shaders/"
	std::string default_path_shaders = project::path +"shaders/";

	// Set standard mesh shader for mesh_drawable
	mesh_drawable::default_shader.load(default_path_shaders +"mesh/mesh.vert.glsl", default_path_shaders +"mesh/mesh.frag.glsl");
	triangles_drawable::default_shader.load(default_path_shaders +"mesh/mesh.vert.glsl", default_path_shaders +"mesh/mesh.frag.glsl");

	// Set default white texture
	image_structure const white_image = image_structure{ 1,1,image_color_type::rgba,{255,255,255,255} };
	mesh_drawable::default_texture.initialize_texture_2d_on_gpu(white_image);
	triangles_drawable::default_texture.initialize_texture_2d_on_gpu(white_image);

	// Set standard uniform color for curve/segment_drawable
	curve_drawable::default_shader.load(default_path_shaders +"single_color/single_color.vert.glsl", default_path_shaders+"single_color/single_color.frag.glsl");
}





//Callback functions
void window_size_callback(GLFWwindow* window, int width, int height);
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);
void keyboard_callback(GLFWwindow* window, int key, int, int action, int mods);

// Standard initialization procedure
window_structure standard_window_initialization()
{
	// Initialize GLFW and create window
	// ***************************************************** //

	// First initialize GLFW
	game_.window.initialize_glfw();

	// Compute initial window width and height
	int window_width = int(project::initial_window_size_width);
	int window_height = int(project::initial_window_size_height);
	if(project::initial_window_size_width<1)
		window_width = project::initial_window_size_width * game_.window.monitor_width();
	if(project::initial_window_size_height<1)
		window_height = project::initial_window_size_height * game_.window.monitor_height();

	// Create the window using GLFW
	window_structure window;
	window.create_window(window_width, window_height, "CGP Display", CGP_OPENGL_VERSION_MAJOR, CGP_OPENGL_VERSION_MINOR);
	window.set_full_screen();


	// Display information
	// ***************************************************** //

	// Display window size
	std::cout << "\nWindow (" << window.width << "px x " << window.height << "px) created" << std::endl;
	std::cout << "Monitor: " << glfwGetMonitorName(window.monitor) << " - Resolution (" << window.screen_resolution_width << "x" << window.screen_resolution_height << ")\n" << std::endl;

	// Display debug information on command line
	std::cout << "OpenGL Information:" << std::endl;
	std::cout << cgp::opengl_info_display() << std::endl;

	// Initialize ImGUI
	// ***************************************************** //
	cgp::imgui_init(window.glfw_window);

	// Set the callback functions for the inputs
	glfwSetMouseButtonCallback(window.glfw_window, mouse_click_callback); // Event called when a button of the mouse is clicked/released
	glfwSetCursorPosCallback(window.glfw_window, mouse_move_callback);    // Event called when the mouse is moved
	glfwSetWindowSizeCallback(window.glfw_window, window_size_callback);  // Event called when the window is rescaled        
	glfwSetKeyCallback(window.glfw_window, keyboard_callback);            // Event called when a keyboard touch is pressed/released
	glfwSetScrollCallback(window.glfw_window, mouse_scroll_callback);     // Event called when scrolling the mouse
	
	return window;
}




// This function is called everytime the window is resized
void window_size_callback(GLFWwindow*, int width, int height)
{
	game_.window.width = width;
	game_.window.height = height;
}

// This function is called everytime the mouse is moved
void mouse_move_callback(GLFWwindow* /*window*/, double xpos, double ypos)
{
	if (render_menu)
		return;
	vec2 const pos_relative = game_.window.convert_pixel_to_relative_coordinates({ xpos, ypos });
	game_.inputs.mouse.position.update(pos_relative);
	game_.mouse_move_event();
}

// This function is called everytime a mouse button is clicked/released
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	
	if (render_menu){
		render_menu = false;
		return;
	}

	if (game_over){
		reset_game = true;
		return;
	}
	game_.inputs.mouse.click.update_from_glfw_click(button, action);
	game_.mouse_click_event();
}

// This function is called everytime the mouse is scrolled
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

	game_.inputs.mouse.scroll = yoffset;
	// game_.mouse_scroll_event();
}

// This function is called everytime a keyboard touch is pressed/released
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	bool imgui_capture_keyboard = ImGui::GetIO().WantCaptureKeyboard;

	
	
	if(!imgui_capture_keyboard){
		if (render_menu && menu_timer >= menu_timeout){
			render_menu = false;
			menu_timer = 0;
		}
		game_.inputs.keyboard.update_from_glfw_key(key, action);
		game_.keyboard_event();

		if (key == GLFW_KEY_P && menu_timer >= menu_timeout){
			menu_timer = 0;
			render_menu = true;
		}

		if (game_over && restart_game_timer >= restart_game_timeout){
			reset_game = true;
			restart_game_timer = 0;
		}
		// Press 'F' for full screen mode
		if (key == GLFW_KEY_F && action == GLFW_PRESS && game_.inputs.keyboard.shift) {
			game_.window.is_full_screen = !game_.window.is_full_screen;
			if (game_.window.is_full_screen)
				game_.window.set_full_screen();
			else
				game_.window.set_windowed_screen();
		}
		// Press 'V' for camera frame/view matrix debug
		if (key == GLFW_KEY_V && action == GLFW_PRESS && game_.inputs.keyboard.shift) {
			auto const camera_model = game_.camera_control.camera_model;
			std::cout << "\nDebug camera (position = " << str(camera_model.position()) << "):\n" << std::endl;
			std::cout << "  Frame matrix:" << std::endl;
			std::cout << str_pretty(camera_model.matrix_frame()) << std::endl;
			std::cout << "  View matrix:" << std::endl;
			std::cout << str_pretty(camera_model.matrix_view()) << std::endl;

		}
	}

}

void display_gui_default()
{
	std::string fps_txt = str(fps_record.fps)+" fps";
	// ImGui::SetWindowSize(ImVec2{80,80});

	if(game_.inputs.keyboard.ctrl)
		fps_txt += " [ctrl]";
	if(game_.inputs.keyboard.shift)
		fps_txt += " [shift]";

	ImGui::Text( fps_txt.c_str(), "%s" );
	if(ImGui::CollapsingHeader("Window")) {
		ImGui::Indent();
#ifndef __EMSCRIPTEN__
		bool changed_screen_mode = ImGui::Checkbox("Full Screen", &game_.window.is_full_screen);
		if(changed_screen_mode){	
			if (game_.window.is_full_screen)
				game_.window.set_full_screen();
			else
				game_.window.set_windowed_screen();
		}
#endif
		ImGui::SliderFloat("Gui Scale", &project::gui_scale, 0.5f, 2.5f);

#ifndef __EMSCRIPTEN__
		// Arbitrary limits the refresh rate to a maximal frame per seconds.
		//  This limits the risk of having different behaviors when you use different machine. 
		ImGui::Checkbox("FPS limiting",&project::fps_limiting);
		if(project::fps_limiting){
			ImGui::SliderFloat("FPS limit",&project::fps_max, 10, 250);
		}
#endif
		// vsync is the default synchronization of frame refresh with the screen frequency
		//   vsync may or may not be enforced by your GPU driver and OS (on top of the GLFW request).
		//   de-activating vsync may generate arbitrary large FPS depending on your GPU and game_.
		if(ImGui::Checkbox("vsync (screen sync)",&project::vsync)){
			project::vsync==true? glfwSwapInterval(1) : glfwSwapInterval(0); 
		}

		std::string window_size = "Window "+str(game_.window.width)+"px x "+str(game_.window.height)+"px";
		ImGui::Text( window_size.c_str(), "%s" );

		ImGui::Unindent();


		ImGui::Spacing();ImGui::Separator();ImGui::Spacing();
	}
	ImGui::SetWindowSize({100,100});
}


