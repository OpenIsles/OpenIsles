#ifndef _NOSDL_PLAIN_GRAPHIC_H
#define _NOSDL_PLAIN_GRAPHIC_H

#include "graphics/graphic/IPlainGraphic.h"
#include "graphics/graphic/nosdl/NoSDLGraphic.h"
#include "graphics/renderer/IRenderer.h"

/**
 * @brief Einfache Grafik - No-SDL-Implementierung für Tests
 */
class NoSDLPlainGraphic : public virtual NoSDLGraphic, public virtual IPlainGraphic {

public:
    NoSDLPlainGraphic(IRenderer* const renderer, const char* filename) : NoSDLGraphic(renderer, filename) {}
    virtual ~NoSDLPlainGraphic() {}

    virtual void drawAt(int x, int y) {}
    virtual void drawScaledAt(int x, int y, double scale) {}
};

#endif
