#ifndef _GRAPHIC_H
#define _GRAPHIC_H

#include <SDL.h>
#include <SDL_image.h>

class Graphic {

private:
	/**
	 * @brief Dateiname der Grafik
	 */
	const char* filename;

	/**
	 * @brief Breite der Grafik
	 */
	unsigned int width;

	/**
	 * @brief Höhe der Grafik
	 */
	unsigned int height;

	/**
	 * @brief SDL-Texture der geladenen Grafik
	 */
	SDL_Texture* texture;

public:
	Graphic(const char* filename);
	~Graphic();

	/**
	 * @brief Liefert die SDL-Texture zurück
	 * @return SDL-Texture
	 */
	SDL_Texture* getTexture() const {
		return texture;
	}

};

#endif
