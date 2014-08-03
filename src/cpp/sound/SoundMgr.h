#ifndef _SOUND_MGR_H
#define _SOUND_MGR_H

#include <SDL.h>
#include <SDL_mixer.h>

class SoundMgr {

private:
	Mix_Music* backgroundMusic = nullptr;

	// TODO wird später die Sounds alle verwalten

public:
	SoundMgr();
	~SoundMgr();

	/**
	 * @brief aktiviert die Hintergrundmusik
	 */
	void enableMusic();
    
    /**
	 * @brief deaktiviert die Hintergrundmusik
	 */
	void disableMusic();

private:
	/**
	 * @brief Lädt eine Musik-Datei
	 * @param file Dateiname
	 * @return Zeiger auf ein SDL-Mix_Music-Objekt
	 */
	Mix_Music* loadMusic(const char* file);
};

#endif
