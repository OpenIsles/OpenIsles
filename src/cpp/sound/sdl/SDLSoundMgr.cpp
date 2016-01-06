#include <stdexcept>
#include "global.h"
#include "sound/sdl/SDLSoundMgr.h"

SDLSoundMgr::SDLSoundMgr() {
    // Library initialisieren
    if ((Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG) != MIX_INIT_OGG) {
        Log::error(_("Could not init SDL-mixer: %s"), Mix_GetError());
        throw std::runtime_error("SDL Mixer could not be initialized");
    }
    atexit(Mix_Quit);

    // Audio-Device öffnen
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        Log::error(_("Could not open audio device: %s"), Mix_GetError());
        throw std::runtime_error("Could not open audio device");
    }

    // Sounds laden
    backgroundMusic = loadMusic("data/audio/Greensleeves.ogg");
}

SDLSoundMgr::~SDLSoundMgr() {
    // Sounds wegräumen
    Mix_FreeMusic(backgroundMusic);
    backgroundMusic = nullptr;

    // Audio-Device schließen
    Mix_CloseAudio();

    // Library runterfahren
    Mix_Quit();
}

Mix_Music* SDLSoundMgr::loadMusic(const char* file) {
    Mix_Music* mixMusic = Mix_LoadMUS(file);
    if (mixMusic == nullptr) {
        Log::error(_("Could not load music '%s': %s"), file, Mix_GetError());
        throw std::runtime_error("Could not load music");
    }

    Log::info(_("Loaded music '%s'."), file);
    return mixMusic;
}

void SDLSoundMgr::enableMusic() {
    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(backgroundMusic, -1);
    }
}

void SDLSoundMgr::disableMusic() {
    if (Mix_PlayingMusic() != 0) {
        Mix_HaltMusic();
    }
}
