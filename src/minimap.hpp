#pragma once

#include <vector>
#include "cgp/cgp.hpp"

using namespace cgp;

class minimap{

    public:
        std::vector<std::vector<char>> map;

        int prev_player_x = -1;
        int prev_player_z = -1;

        int height;
        int width;

        minimap();

        void update(int player_x, int player_z);

        void add_weapons(std::vector<vec3>& weapon_pos);

        void add_medicine(std::vector<vec3>& medicine_position);

        void print();
};