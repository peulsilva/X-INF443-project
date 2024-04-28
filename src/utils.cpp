#include "cgp/cgp.hpp"
#include "utils.hpp"

vec3 utils::remove_y_direction(vec3 v){
    vec3 v_without_y = mat3{{1,0,0},{0,0,0}, {0,0,1}} * v;
    if (norm(v_without_y) < 1e-3)
        return {0,0,0};
    return normalize(v_without_y);
}