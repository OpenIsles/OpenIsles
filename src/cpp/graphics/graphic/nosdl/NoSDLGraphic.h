#ifndef _NOSDL_GRAPHIC_H
#define _NOSDL_GRAPHIC_H

#include "graphics/graphic/IGraphic.h"
#include "graphics/renderer/IRenderer.h"

/**
 * @brief Basisklasse für alle Grafiken. No-SDL-Implementierung für Tests
 */
class NoSDLGraphic : public virtual IGraphic {

public:
	NoSDLGraphic(IRenderer* const renderer, const char* filename, int width, int height,
		         unsigned char mapWidth, unsigned char mapHeight) {
        this->width = width;
        this->height = height;
		this->mapWidth = mapWidth;
		this->mapHeight = mapHeight;
	}

    NoSDLGraphic(IRenderer* const renderer, const char* filename, unsigned char mapWidth, unsigned char mapHeight) :
		NoSDLGraphic(renderer, filename, 0, 0, mapWidth, mapHeight) {}

	NoSDLGraphic(
		IRenderer* const renderer, const IGraphic& srcGraphic, const Rect& srcRect,
		unsigned char mapWidth, unsigned char mapHeight) :
			NoSDLGraphic(renderer, "", 0, 0, mapWidth, mapHeight) {}

	virtual ~NoSDLGraphic() {}

	void getPixel(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) const {
		r = g = b = a = 0;
	}

	void drawAt(int x, int y) const {}
    void drawScaledAt(int x, int y, double scale) const {}
	void drawShadowScaledAt(int x, int y, double scale) const {}
	void draw(Rect* rectSource, Rect* rectDestination, int drawingFlags, uint32_t sdlTicks) const {}
};

#endif