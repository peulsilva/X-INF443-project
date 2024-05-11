#include "minimap.hpp"
#include "constants.hpp"

using namespace cgp;

minimap::minimap(){

    height = constants::MINIMAP_HEIGHT + 2;
    width = constants::MINIMAP_WIDTH + 2;

    map = std::vector<std::vector<char>>(
        height, 
        std::vector<char>(width, ' ')
    );

    for (int i = 0; i < height; ++i) {
        map[i][0] = '|';
        map[i][width - 1] = '|';
    }
    for (int j = 0; j < width; ++j) {
        map[0][j] = '-';
        map[height - 1][j] = '-';
    }
    map[0][0] = map[0][width - 1] = map[height - 1][0] = map[height - 1][width - 1] = '+';
}

void minimap::update(int player_x, int player_z){
    char dir;
    if (prev_player_x != -1 && prev_player_z != -1) {

        int prev_scaled_x = (-prev_player_x + constants::WORLD_SIZE)* height / (2*constants::WORLD_SIZE) ;
        int prev_scaled_z = (prev_player_z + constants::WORLD_SIZE) * width / (2*constants::WORLD_SIZE);

        prev_scaled_x = std::max(prev_scaled_x, 1);
        prev_scaled_x = std::min(height - 1, prev_scaled_x);

        prev_scaled_z = std::max(prev_scaled_z, 1);
        prev_scaled_z = std::min(width - 1, prev_scaled_z);

        map[prev_scaled_x][prev_scaled_z] = ' ';
    }

    for (int i =0 ; i < map.size(); i++){
        for (int j = 0; j < map[0].size(); j++){
            if (map[i][j] == 'P')
                map[i][j] = ' '; 
        }
    }

    int scaled_x = (-player_x + constants::WORLD_SIZE)* height / (2*constants::WORLD_SIZE) ;
    int scaled_z = (player_z + constants::WORLD_SIZE) * width / (2*constants::WORLD_SIZE);


    scaled_x = std::max(scaled_x, 1);
    scaled_x = std::min(height - 2, scaled_x);

    scaled_z = std::max(scaled_z, 1);
    scaled_z = std::min(width - 2, scaled_z);

    // verificar scaled_x e scaled_z
    // if (prev_player_x > player_x)
    //     map[scaled_x][scaled_z] = 'v';
    // else if(prev_player_x < player_x)
    //     map[scaled_x][scaled_z] = '^';
    // else if(prev_player_z > player_z)
    //     map[scaled_x][scaled_z] = '<';
    // else if(prev_player_z > player_z)
    //     map[scaled_x][scaled_z] = '>';
    // else{
    //     map[scaled_x][scaled_z] = dir;
    // }
    
    map[scaled_x][scaled_z] = 'P';
    prev_player_x = player_x;
    prev_player_z = player_z;

}

void minimap::print(){
    std::string text;
    for (const auto& row : map) {
        for (char cell : row) {
            text += cell;

            
        }

        text += '\n';
    }
    ImGui::Text(text.c_str());
}

void minimap::add_weapons(std::vector<vec3>& weapon_pos){
    for (auto& pos: weapon_pos){

        int scaled_x = (-pos.x + constants::WORLD_SIZE)* height / (2*constants::WORLD_SIZE) ;
        int scaled_z = (pos.z + constants::WORLD_SIZE) * width / (2*constants::WORLD_SIZE);

        scaled_x = std::max(scaled_x, 1);
        scaled_x = std::min(height - 2, scaled_x);

        scaled_z = std::max(scaled_z, 1);
        scaled_z = std::min(width - 2, scaled_z);

        map[scaled_x][scaled_z] ='w';
    }

}

void minimap::add_medicine(std::vector<vec3>& medicine_position){
    for (auto& pos: medicine_position){

        int scaled_x = (-pos.x + constants::WORLD_SIZE)* height / (2*constants::WORLD_SIZE) ;
        int scaled_z = (pos.z + constants::WORLD_SIZE) * width / (2*constants::WORLD_SIZE);

        scaled_x = std::max(scaled_x, 1);
        scaled_x = std::min(height - 2, scaled_x);

        scaled_z = std::max(scaled_z, 1);
        scaled_z = std::min(width - 2, scaled_z);

        map[scaled_x][scaled_z] ='+';
    }
}