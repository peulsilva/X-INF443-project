#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"

using namespace cgp;

struct camera_controller: camera_controller_first_person_euler
{

	float mouse_sensitivity = 1.;
	float keyboard_sensitivity = 5.0;
	float const pi = std::atan(1) * 4;

	camera_controller();

	void action_mouse_move(mat4 &camera_matrix_view);

	// void action_keyboard(mat4&, bool&);

	void idle_frame(mat4& camera_matrix_view);

	void set_position(vec3& position);

	void deactivate();
	void activate();

	private:
	    bool cursor_enabled = false;
};