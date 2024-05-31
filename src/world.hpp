#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "unordered_map"
#include "utils.hpp"

using namespace cgp;
class world{
    public :
        mesh_drawable ground;
        mesh_drawable house;
        vec3 house_position;
        void initialize(std::unordered_map<std::string, vec3>& obstacle_positions);

        void draw(const environment_structure& env, bool display_wireframe);

        // static vec3 collide_with_house(vec3 house_pos, vec3 pos, vec3 moving_direction);
};