#ifndef _SDL_GRAPHIC_H
#define _SDL_GRAPHIC_H

#include <SDL.h>
#include <SDL_image.h>
#include "graphics/graphic/IGraphic.h"
#include "graphics/renderer/IRenderer.h"

/**
 * @brief SDL-Implementierung für die Einzelgrafik
 *
 * TODO der Einfachheit halber wird `textureMasked` immer erzeugt. Für GUI- oder Tile-Grafiken z.B. könnten wir uns das aber sparen.
 */
class SDLGraphic : public virtual IGraphic {

protected: // Dependencies
	IRenderer* const renderer;

protected:
	/**
	 * @brief SDL-Texture der geladenen Grafik
	 */
	SDL_Texture* texture;

	/**
     * @brief SDL-Texture der maskierten Grafik zurück. Diese wird verwendet, wenn ein neues Gebäude
     * positioniert wird.
     */
    SDL_Texture* textureMasked;

	/**
	 * @brief SDL-Surface
	 */
	SDL_Surface* surface;

public:
    /**
     * @brief Konstruktor. Lädt die Grafik aus einer Datei und setzt Map-Koordinatengröße auf 0
	 * @param renderer (Dependency)
     * @param filename Dateiname der zu ladenden Grafik
     */
    SDLGraphic(IRenderer* const renderer, const char* filename);

	/**
     * @brief Konstruktor. Lädt die Grafik aus einer Datei
	 * @param renderer (Dependency)
     * @param filename Dateiname der zu ladenden Grafik
	 * @param mapWidth Breite der Grafik in Map-Koordinaten
	 * @param mapHeight Höhe der Grafik in Map-Koordinaten
     */
	SDLGraphic(IRenderer* const renderer, const char* filename, unsigned char mapWidth, unsigned char mapHeight);

	/**
     * @brief Konstruktor. Erzeugt eine Grafik als Teil einer anderen Grafik und setzt Map-Koordinatengröße auf 0
	 * @param renderer (Dependency)
     * @param graphic Quellgrafik
	 * @param rect Rechteck innerhalb der Quellgrafik, die den Bereich markiert, welcher die neue Grafik auszeichnet
     */
    SDLGraphic(IRenderer* const renderer, const SDLGraphic& srcGraphic, const Rect& srcRect);

	/**
     * @brief Konstruktor. Erzeugt eine Grafik als Teil einer anderen Grafik
	 * @param renderer (Dependency)
     * @param srcGraphic Quellgrafik
	 * @param srcRect Rechteck innerhalb der Quellgrafik, die den Bereich markiert, welcher die neue Grafik auszeichnet
	 * @param mapWidth Breite der Grafik in Map-Koordinaten
	 * @param mapHeight Höhe der Grafik in Map-Koordinaten
     */
	SDLGraphic(
		IRenderer* const renderer, const SDLGraphic& srcGraphic, const Rect& srcRect,
		unsigned char mapWidth, unsigned char mapHeight);

    /**
     * @brief Destruktor. Entlädt die Grafik und gibt Speicher frei.
     */
	virtual ~SDLGraphic();

	virtual void getPixel(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) const;
    virtual void drawAt(int x, int y) const;
    virtual void drawScaledAt(int x, int y, double scale) const;
	virtual void draw(Rect* rectSource, Rect* rectDestination, int drawingFlags, uint32_t sdlTicks) const;

private:
	/**
	 * @brief Erzeugt die Texturen. Erwartet, dass `surface` bereits mit der Grafik befüllt ist.
	 */
	void createTextures();

    /**
     * @brief Erzeugt `textureMasked`. Erwartet, dass `surface` bereits mit der Grafik befüllt ist.
     */
    void createMaskedTexture();

};

#endif
