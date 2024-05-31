#include "world.hpp"
#include "constants.hpp"


void world::initialize(std::unordered_map<std::string, vec3>& obstacle_positions){
    ground.initialize_data_on_gpu(mesh_primitive_quadrangle(
		vec3{ -1.0f, 0.0f, -1.0f }, 
		vec3{ -1.0f, 0.0f, 1.0f }, 
		vec3{ 1.0f, 0.0f, 1.0f }, 
		vec3{ 1.0f, 0.0f, -1.0f }
	));
	ground.model.translation = { 0.0f, 0.0f, 0.0f }; // No translation needed for y-axis up
	ground.texture.load_and_initialize_texture_2d_on_gpu(
		project::path + "assets/grass.jpg",
		GL_REPEAT,
		GL_REPEAT
	);
	ground.model.scaling = constants::WORLD_SIZE;

    mesh tree_mesh = create_tree();
	tree.initialize_data_on_gpu(tree_mesh);
    tree.model.rotation = rotation_transform::from_frame_transform(
        {0,1,0},
        {0,0,1},
        {0,0,1},
        {0,1,0}
    );
    
    std::vector<double> scales = {3.2, 2, 3, 2.6, 2.8, 3.4, 4, 5.2};
    int idx = 0;
    for (auto [name, pos]: obstacle_positions){
        idx = (idx+1)%scales.size(); 
        if (name.substr(0, 4).compare("tree") == 0){
            trees.push_back({pos, scales[idx]});
        }
    }

    rock.initialize_data_on_gpu(
        mesh_load_file_obj("assets/world/rock4.obj")
    );
    rock.texture.load_and_initialize_texture_2d_on_gpu(
        "assets/world/texture2.png"
    );

    scales = {1};
    idx = 0;
    for (auto [name, pos]: obstacle_positions){
        idx = (idx+1)%scales.size(); 
        if (name.substr(0, 4).compare("rock") == 0){
            pos.y -= 0.4;
            pos.z += 15.5;
            pos.x -= 5.4;
            rocks.push_back({pos, scales[idx]});
        }
    }

    car.initialize_data_on_gpu(mesh_load_file_obj("assets/world/Rusted Car.obj"));
    car.texture.load_and_initialize_texture_2d_on_gpu( "assets/world/Car Uv.png");
    car.model.translation = obstacle_positions["car"] + vec3{0,0,2.2};
    car.model.set_scaling(1/3.);

    house.initialize_data_on_gpu(mesh_load_file_obj("assets/world/old house.obj"));
    house.texture.load_and_initialize_texture_2d_on_gpu("assets/world/SAN_RUMAHGEN05.png");
    house.model.rotation = rotation_transform::from_frame_transform(
        {0,1,0},
        {0,0,1},
        {0,0,1},
        {0,1,0}
    );
    house.model.translation = obstacle_positions["house"] - vec3{0, 0 ,0.5};
}

void world::draw(const environment_structure& env, bool display_wireframe){
    if (display_wireframe){
        cgp::draw_wireframe(ground,env);

        for (auto& [pos, size]: trees){
            tree.model.translation = pos;
            tree.model.scaling = size;
            cgp::draw_wireframe(tree, env);
        }

        for (auto& [pos, size]: rocks){
            rock.model.translation = pos;
            rock.model.scaling = size;
            cgp::draw_wireframe(rock, env);
        }
    }
        
    else{
        for (auto& [pos, size]: trees){
            tree.model.translation = pos;
            tree.model.scaling = size;
            cgp::draw(tree, env);    
        }
        for (auto& [pos, size]: rocks){
            rock.model.translation = pos;
            rock.model.scaling = size;
            cgp::draw(rock, env);
        }

        cgp::draw(house, env);
        cgp::draw(ground, env);
        cgp::draw(car, env);
    }
}

