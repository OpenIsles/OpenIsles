#ifndef _SOUND_MGR_H
#define _SOUND_MGR_H

#include <SDL.h>
#include <SDL_mixer.h>
#include "sound/ISoundMgr.h"

class SDLSoundMgr : public ISoundMgr {

private:
    Mix_Music* backgroundMusic = nullptr;

    // TODO wird später die Sounds alle verwalten

public:
    SDLSoundMgr();
    ~SDLSoundMgr();

    /**
     * @brief aktiviert die Hintergrundmusik
     */
    virtual void enableMusic();
    
    /**
     * @brief deaktiviert die Hintergrundmusik
     */
    virtual void disableMusic();

private:
    /**
     * @brief Lädt eine Musik-Datei
     * @param file Dateiname
     * @return Zeiger auf ein SDL-Mix_Music-Objekt
     */
    Mix_Music* loadMusic(const char* file);
};

#endif
