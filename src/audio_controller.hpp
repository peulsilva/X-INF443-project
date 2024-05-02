#pragma once
#include <thread>
#include <iostream>

struct listeners {
    bool shoot_rifle = false;
    bool shoot_handgun = false;
    bool shoot_shotgun = false;
    bool shoot_smg = false;
    bool empty_clip = false;
    bool reload_rifle = false;
    bool reload_handgun = false;
    bool reload_shotgun = false;
    bool take_hit = false;
    bool footsteps = false;
    bool death = false;
};

extern listeners lists;

void init_sound_effects();

void init_sfx(const char* wav_path, int duration_ms, bool* listener, bool on_repeat);