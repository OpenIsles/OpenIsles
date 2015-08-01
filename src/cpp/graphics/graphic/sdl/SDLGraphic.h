#ifndef _SDL_GRAPHIC_H
#define _SDL_GRAPHIC_H

#include <SDL.h>
#include <SDL_image.h>
#include <functional>
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
     * @brief SDL-Texture der maskierten Grafik. Diese wird verwendet, wenn ein neues Gebäude positioniert wird.
     */
    SDL_Texture* textureMasked;

	/**
     * @brief SDL-Texture der Schatten-Grafik. Diese wird im Panel verwendet, wenn ein Gebäude gebaut wird.
     */
    SDL_Texture* textureShadow;

	/**
	 * @brief SDL-Surface
	 */
	SDL_Surface* surface;

public:
    /**
     * @brief Konstruktor. Lädt die Grafik aus einer Datei
	 * @param renderer (Dependency)
     * @param filename Dateiname der zu ladenden Grafik
     */
    SDLGraphic(IRenderer* const renderer, const char* filename);

	/**
     * @brief Konstruktor. Erzeugt eine Grafik als Teil einer anderen Grafik
	 * @param renderer (Dependency)
     * @param srcGraphic Quellgrafik
	 * @param srcRect Rechteck innerhalb der Quellgrafik, die den Bereich markiert, welcher die neue Grafik auszeichnet
     */
	SDLGraphic(IRenderer* const renderer, const SDLGraphic& srcGraphic, const Rect& srcRect);

    /**
     * @brief Destruktor. Entlädt die Grafik und gibt Speicher frei.
     */
	virtual ~SDLGraphic();

	virtual void getPixel(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) const;
    virtual void drawAt(int x, int y) const;
    virtual void drawScaledAt(int x, int y, double scale) const;
	virtual void drawShadowAt(int x, int y) const;
	virtual void drawShadowScaledAt(int x, int y, double scale) const;
	virtual void draw(Rect* rectSource, Rect* rectDestination, int drawingFlags) const;

private:
	/**
	 * @brief Erzeugt die Texturen. Erwartet, dass `surface` bereits mit der Grafik befüllt ist.
	 */
	void createTextures();

	/**
	 * @brief Erzeugt eine alternative Textur als der Grafik
	 * @param fillPixelsFunction Funktion, die die Pixel malt. Als Parameter erhält sie einen Buffer,
	 *                           der mit Pixel gefüllt werden muss.
	 * @return Zeiger auf die erzeugte `SDL_Texture` oder `nullptr`, wenn die Grafik das falsche Format hat
	 */
	SDL_Texture* createAlternativeTexture(std::function<void(unsigned char* pixelBuffer)> fillPixelsFunction);

	/**
	 * @brief Interner Helper, der die Grafik an eine bestimmte Stelle mit definierter Zielgröße zeichnet.
	 * Es wird normal (keine drawingFlags) gezeichnet.
	 *
	 * @param rectDestination SDL-Rechteck mit Zielkoordinaten und -Größe
	 */
	void drawAt(const SDL_Rect& rectDestination) const;

};

#endif
