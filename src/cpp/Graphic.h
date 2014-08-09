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

	/**
	 * @brief SDL-Surface
	 */
	SDL_Surface* surface;

public:
	/**
	 * @brief Konstruktor für kartenspezifische Grafiken, die eine Größe in Kacheln haben
	 * @param filename Dateiname der zu ladenden Grafik
	 * @param mapWidth Breite in mapCoords
	 * @param mapHeight Höhe in mapCoords
	 */
	Graphic(const char* filename, unsigned char mapWidth, unsigned char mapHeight);

	/**
	 * @brief Konstruktor für allgemeine Grafiken
	 * @param filename Dateiname der zu ladenden Grafik
	 */
	Graphic(const char* filename);

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
    
    /**
	 * @brief Liefert die SDL-Texture für die maskierte Grafik zurück. Diese wird verwendet, wenn ein neues Gebäude
     * positioniert wird.
	 * @return SDL-Texture
	 */
	SDL_Texture* getTextureMasked() const;

	/**
	 * @brief Liefert das SDL-Surface zurück
	 * @return SDL-Surface
	 */
	SDL_Surface* getSurface() const {
		return surface;
	}

	/**
	 * @brief Liest die Farbwerte eines bestimmten Pixels aus und setzt sie in die r-, g-, b- und a-Parameter.
	 * @param x x-Koordinate (IN)
	 * @param y y-Koordinate (IN)
	 * @param r wird auf den Rot-Wert des Pixels gesetzt (OUT)
	 * @param g wird auf den Grün-Wert des Pixels gesetzt (OUT)
	 * @param b wird auf den Blau-Wert des Pixels gesetzt (OUT)
	 * @param a wird auf den Alpha-Wert des Pixels gesetzt (OUT) 0 = voll transparent, 255 = absolut undurchsichtig
	 */
	void getPixel(int x, int y, Uint8* r, Uint8* g, Uint8* b, Uint8* a);
    
    // TODO nur vorübergehend hier. Grafiken manipulieren sollte eigentlich keiner von außen können!
    /**
	 * @brief Setzt die Farbwerte eines bestimmten Pixels.
	 * @param x x-Koordinate
	 * @param y y-Koordinate
	 * @param r Rot-Wert des Pixels
	 * @param g Grün-Wert des Pixels
	 * @param b Blau-Wert des Pixels
	 * @param a Alpha-Wert des Pixels. 0 = voll transparent, 255 = absolut undurchsichtig
	 */
	void setPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

};

#endif
