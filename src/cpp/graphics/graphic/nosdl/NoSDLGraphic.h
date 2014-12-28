#ifndef _NOSDL_GRAPHIC_H
#define _NOSDL_GRAPHIC_H

#include "graphics/graphic/IGraphic.h"
#include "graphics/renderer/IRenderer.h"

/**
 * @brief Basisklasse für alle Grafiken. No-SDL-Implementierung für Tests
 */
class NoSDLGraphic : public virtual IGraphic {

public:
    NoSDLGraphic(IRenderer* const renderer, const char* filename) {}
	virtual ~NoSDLGraphic() {}


	virtual int getWidth() const {
		return 0;
	}

	virtual int getHeight() const {
		return 0;
	}

	virtual void getPixel(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
		r = g = b = a = 0;
	}
};

#endif