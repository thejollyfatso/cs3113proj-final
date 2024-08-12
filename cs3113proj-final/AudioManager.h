#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL_mixer.h>
#include <map>
#include <string>

class AudioManager {
public:
    // Constructor
    AudioManager();

    // Destructor
    ~AudioManager();

    // Load all audio files
    bool load_sounds();

    // Play a specific sound
    void play_sound(const std::string& sound_name);
    static void channel_finished(int channel);
    void play_unique_sound(const std::string& sound_name);

private:
    std::map<std::string, Mix_Chunk*> m_sounds;  // Using Mix_Chunk for short sound effects
    static std::map<std::string, int> playing_sounds;
};

#endif // AUDIOMANAGER_H

