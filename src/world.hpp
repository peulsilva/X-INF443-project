#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "unordered_map"
#include "utils.hpp"
#include "tree.hpp"

using namespace cgp;
class world{
    public :
        mesh_drawable ground;
        mesh_drawable tree;
        mesh_drawable rock;
        mesh_drawable car;
        mesh_drawable house;
        std::vector<std::pair<vec3, int>> trees; // position, size
        std::vector<std::pair<vec3, int>> rocks; // position, size

        void initialize(std::unordered_map<std::string, vec3>& obstacle_positions);

        void draw(const environment_structure& env, bool display_wireframe);

        // static vec3 collide_with_house(vec3 house_pos, vec3 pos, vec3 moving_direction);
};