#include "tree.hpp"

using namespace cgp;

const float PI = 3.14159265359f;
mesh create_cylinder_mesh(float radius, float height)
{
    mesh m; 
    int n_segments = 16;
    for (int i = 0; i< n_segments; i++){
        float theta1 = 2.0f * PI * float(i) / float(n_segments);
        float theta2 = 2.0f * PI * float(i + 1) / float(n_segments);

        vec3 p1 = {radius * cos(theta1), radius*sin(theta1), 0};
        vec3 p2 = {radius * cos(theta2), radius*sin(theta2), height};

        m.position.push_back(p1);
        m.position.push_back(p2);
    }

    for(int k=0; k<n_segments; ++k){
        int u00 = 2*k;
        int u01 = (2*k+1)%(2*n_segments);
        int u10 = (2*(k+1))%(2*n_segments);
        int u11 = (2*(k+1)+1) % (2*n_segments);

        uint3 t1 = {u00, u10, u11};
        uint3 t2 = {u00, u11, u01};
        m.connectivity.push_back(t1);
        m.connectivity.push_back(t2);
    }

    m.fill_empty_field();

    return m;
}

mesh create_cone_mesh(float radius, float height, float z_offset)
{
    mesh m; 
    int n_segments = 16;
    int N = 20;


    for(int k=0; k<N; ++k)
    {
        float theta = 2*PI*k/float(N);
        vec3 p = {
            radius*cos(theta), 
            radius*sin(theta), 
            z_offset
        };
        m.position.push_back(p);
    }
    // apex
    m.position.push_back({0,0,height+z_offset});

    // connectivity
    int Ns = N;
    for(int k=0; k<Ns; ++k) {
        m.connectivity.push_back( {k , (k+1)%Ns, Ns} );
    }

    // close the bottom of the cone
    // *************************** //

    // Geometry
    for(int k=0; k<N; ++k)
    {
        float theta = 2*PI*k/float(N);
        vec3 p = {
            radius*std::cos(theta), 
            radius*std::sin(theta), 
            z_offset
        };
        m.position.push_back(p);
    }
    // central position
    m.position.push_back( {0,0,z_offset} );

    // connectivity
    for(int k=0; k<Ns; ++k)
        m.connectivity.push_back( {k+Ns+1, (k+1)%Ns+Ns+1, 2*Ns+1} );

    m.fill_empty_field();
    return m;
}

mesh create_tree()
{
    float h = 0.7f; // trunk height
    float r = 0.1f; // trunk radius

    // Create a brown trunk
    mesh trunk = create_cylinder_mesh(r, h);
    trunk.color.fill({0.4f, 0.3f, 0.3f});


    // Create a green foliage from 3 cones
    mesh foliage = create_cone_mesh(4*r, 6*r, 0.0f);      // base-cone
    foliage.push_back(create_cone_mesh(4*r, 6*r, 2*r));   // middle-cone
    foliage.push_back(create_cone_mesh(4*r, 6*r, 4*r));   // top-cone
    foliage.translate({0,0,h});       // place foliage at the top of the trunk
    foliage.color.fill({0.4f, 0.6f, 0.3f});
       
    // The tree is composed of the trunk and the foliage
    mesh tree = trunk;
    tree.push_back(foliage);

    return tree;
}


