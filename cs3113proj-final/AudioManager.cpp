#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
    load_sounds();
}

AudioManager::~AudioManager() {
    for (auto& sound : m_sounds) {
        Mix_FreeChunk(sound.second);
    }
    Mix_CloseAudio();
}

bool AudioManager::load_sounds() {
    m_sounds["dash"] = Mix_LoadWAV("assets/sound/dash.wav");
    m_sounds["bind"] = Mix_LoadWAV("assets/sound/bind.wav");
    m_sounds["parry"] = Mix_LoadWAV("assets/sound/parry.wav");
    m_sounds["swing"] = Mix_LoadWAV("assets/sound/swing.wav");
    m_sounds["knockback"] = Mix_LoadWAV("assets/sound/knockback_alt.wav");
    m_sounds["decision"] = Mix_LoadWAV("assets/sound/decision.wav");
    m_sounds["adv"] = Mix_LoadWAV("assets/sound/adv.wav");
    m_sounds["hit"] = Mix_LoadWAV("assets/sound/Sword1.wav");
    m_sounds["death"] = Mix_LoadWAV("assets/sound/death3.wav");

    for (const auto& sound : m_sounds) {
        if (sound.second == nullptr) {
            std::cerr << "Failed to load " << sound.first << " sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
            return false;
        }
    }

    Mix_AllocateChannels(16); 
    Mix_Volume(
        -1,        
        MIX_MAX_VOLUME 
    );

    Mix_VolumeChunk(
        m_sounds["dash"],     
        MIX_MAX_VOLUME
    );
    return true;
}

void AudioManager::play_sound(const std::string& sound_name) {
    auto it = m_sounds.find(sound_name);
    if (it != m_sounds.end()) {
        if (Mix_PlayChannel(-1, it->second, 0) == -1) {  // Use Mix_PlayChannel for Mix_Chunk
            std::cerr << "Mix_PlayChannel: " << Mix_GetError() << std::endl;
        }
    }
    else {
        std::cerr << "Sound " << sound_name << " not found!" << std::endl;
    }
}

