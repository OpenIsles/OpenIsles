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
	 * @brief Breite (X-Richtung) der Grafik in Tile-Koordinaten
	 */
	unsigned char mapWidth;

	/**
	 * @brief Höhe (Y-Richtung) der Grafik in Tile-Koordinaten
	 */
	unsigned char mapHeight;

	/**
	 * @brief Breite der Grafik
	 */
	int width;

	/**
	 * @brief Höhe der Grafik
	 */
	int height;

	/**
	 * @brief SDL-Texture der geladenen Grafik
	 */
	SDL_Texture* texture;

public:
	Graphic(const char* filename, unsigned char mapWidth, unsigned char mapHeight);
	~Graphic();

	/**
	 * @brief Liefert die Breite der Grafik in Map-Koordinaten zurück
	 * @return Breite der Grafik in Map-Koordinaten
	 */
	unsigned char getMapWidth() const {
		return mapWidth;
	}

	/**
	 * @brief Liefert die Höhe der Grafik in Map-Koordinaten zurück
	 * @return Breite der Höhe in Map-Koordinaten
	 */
	unsigned char getMapHeight() const {
		return mapHeight;
	}

	/**
	 * @brief Liefert die Breite der Grafik zurück
	 * @return Breite der Grafik
	 */
	int getWidth() const {
		return width;
	}

	/**
	 * @brief Liefert die Höhe der Grafik zurück
	 * @return Höhe der Grafik
	 */
	int getHeight() const {
		return height;
	}

	/**
	 * @brief Liefert die SDL-Texture zurück
	 * @return SDL-Texture
	 */
	SDL_Texture* getTexture() const {
		return texture;
	}

};

#endif
