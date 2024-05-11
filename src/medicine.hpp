#pragma once

#include "environment.hpp"
#include "cgp/cgp.hpp"


class medicine{
    public:

        mesh_drawable object;
        vec3 position;

        medicine(vec3 _position);

        medicine();

        void draw(const environment_structure& env, bool wireframe);


};