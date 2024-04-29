#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.hpp"
#include "audio_controller.hpp"


listeners lists;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

void init_sound_effects(){
	std::thread rifle_shot(init_sfx, "assets/audio/ak-47-89833.wav", 500, &(lists.shoot_rifle), false);
	rifle_shot.detach();

    std::thread footsteps(init_sfx, "assets/audio/concrete-footsteps-6752.mp3", 500, &(lists.footsteps), false);
	footsteps.detach();

    std::thread take_hit(init_sfx, "assets/audio/male_hurt7-48124.mp3", 1500, &(lists.take_hit), false);
	take_hit.detach();

    std::thread death(init_sfx, "assets/audio/alphascream001-98301.mp3", 1500, &(lists.death), false);
	death.detach();

    std::thread empty_clip(init_sfx, "assets/audio/rifle-clip-empty-98832.mp3", 500, &(lists.empty_clip), false);
	empty_clip.detach();

    std::thread reload(init_sfx, "assets/audio/machine-gun-reload-81593.mp3", 1000, &(lists.reload), false);
	reload.detach();
    // std::thread hit(init_sfx, "../assets/bullet_hit.wav", 100, &(lists.hit), false);
	// hit.detach();

	// std::thread kill(init_sfx, "../assets/creeper_death.wav", 1000, &(lists.kill), false);
	// kill.detach();

	// std::thread death(init_sfx, "../assets/death_bell.wav", 4500, &(lists.death), false);
	// death.detach();

	// std::thread place_block(init_sfx, "../assets/place_block.wav", 140, &(lists.place_block), false);
	// place_block.detach();

	// std::thread background(init_sfx, "../assets/background_song.wav", 138000, &(lists.background), true);
	// background.detach();

	// std::thread background_creative(init_sfx, "../assets/background_song_calm.mp3", 175000, &(lists.background_creative), true);
	// background_creative.detach();
}

void init_sfx(const char* wav_path, int duration_ms, bool* listener, bool on_repeat) {
    for(;;){
        // #ifdef _WIN32
        std::cout << ""; // dont know why but this is needed to make audio run on windows ...
        // #endif //_WIN32
		if (!*listener) continue;
		if (!on_repeat) *listener = false;

        ma_result result;
        ma_decoder decoder;
        result = ma_decoder_init_file(wav_path, NULL, &decoder);
        if (result != MA_SUCCESS) {
            std::cerr << "Failed to initialize decoder" << std::endl;
            // return;
        }
        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = decoder.outputFormat;
        deviceConfig.playback.channels = decoder.outputChannels;
        deviceConfig.sampleRate = decoder.outputSampleRate;
        deviceConfig.dataCallback = data_callback;
        deviceConfig.pUserData = &decoder;
        ma_device device;

        result = ma_device_init(NULL, &deviceConfig, &device);

        if (result != MA_SUCCESS) {
            std::cerr << "Failed to initialize playback device" << std::endl;
            ma_decoder_uninit(&decoder);
            return;
        }

 	   	result = ma_device_start(&device);

 	   	if (result != MA_SUCCESS) {
 	   	    std::cerr << "Failed to start playback device" << std::endl;
 	   	    ma_device_uninit(&device);
 	   	    ma_decoder_uninit(&decoder);
 	   	    return;
 	   	}

 	   	// Sleep for the duration of the sound
 	   	std::chrono::milliseconds duration(duration_ms);
 	   	std::this_thread::sleep_for(duration);
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        if (!on_repeat) *listener = false;
	}

    std::cout << "started sounds " << std::endl;
}


void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}