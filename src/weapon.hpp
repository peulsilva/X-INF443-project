
#include "cgp/cgp.hpp"
#include "camera.hpp"
#include <unordered_map>

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

        float timeout_factor;

        int damage;

        float timeout;
        int counter = 0;

        std::unordered_map<weapon_type, std::string> weapon_name_map = {
            {rifle, "Rifle"},
            {handgun, "Handgun"},
            {shotgun, "Shotgun"},
            {smg, "SMG"}
        };
        

        weapon(weapon_type type);
        weapon();

        std::string get_weapon_name();

        bool shoot();
        void reload();

        void set_fps(int fps);

        int get_damage(vec3 zombie_pos, vec3 player_pos);

        void draw(const environment_structure& env, camera_controller_first_person_euler* camera, bool wireframe, bool is_aiming);


        void draw_on_scene(const environment_structure& env , vec3 position);

        static weapon_type choose_random_weapon();

};