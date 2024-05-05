
#include "cgp/cgp.hpp"
#include "camera.hpp"
#include "unordered_map"

using namespace cgp;

enum weapon_type{
    handgun,
    shotgun,
    rifle,
    smg
};




class weapon{
    public:

        weapon_type type;

        int bullets_in_clip;
        int total_bullets;
        int max_bullets_in_clip;
        mesh_drawable object; 

        int damage;

        int timeout;
        int counter = 0;
        

        weapon(weapon_type type);
        weapon();

        bool shoot();
        void reload();

        int get_damage(vec3 zombie_pos, vec3 player_pos);

        void draw(const environment_structure& env, camera_controller_first_person_euler* camera, bool wireframe, bool is_aiming);


        void draw_on_scene(const environment_structure& env , vec3 position);

        static weapon_type choose_random_weapon();

};