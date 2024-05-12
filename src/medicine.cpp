#include "medicine.hpp"

medicine::medicine(vec3 _position){
    object.initialize_data_on_gpu(
        mesh_load_file_obj("assets/pills_bottle1.obj")
    );

	object.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/pills_bottle1_material_Base_Color.png");

    position= _position;

}

medicine::medicine(){}

void medicine::draw(
    const environment_structure& env, 
    bool wireframe
){
    object.model.translation = {position.x, 0.7, position.z};

    object.model.set_scaling(2e-2);

    cgp::draw(object, env);
}