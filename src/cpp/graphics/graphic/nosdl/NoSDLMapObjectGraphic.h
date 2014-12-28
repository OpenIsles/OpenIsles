#ifndef _NOSDL_MAP_OBJECT_GRAPHIC_H
#define _NOSDL_MAP_OBJECT_GRAPHIC_H

#include "graphics/graphic/AbstractMapObjectGraphic.h"
#include "graphics/graphic/IMapObjectGraphic.h"
#include "graphics/graphic/nosdl/NoSDLPlainGraphic.h"
#include "graphics/renderer/IRenderer.h"
#include "utils/Rect.h"

/**
 * @brief Grafik eines MapObjects - No-SDL-Implementierung für Tests
 */
class NoSDLMapObjectGraphic : public virtual NoSDLPlainGraphic, public virtual AbstractMapObjectGraphic {

public:
    NoSDLMapObjectGraphic(
        IRenderer* const renderer, const char* filename, unsigned char mapWidth, unsigned char mapHeight) :
            NoSDLGraphic(renderer, filename),
            NoSDLPlainGraphic(renderer, filename),
            AbstractMapObjectGraphic(mapWidth, mapHeight) {}

    virtual ~NoSDLMapObjectGraphic() {}

    virtual void draw(Rect* rectSource, Rect* rectDestination, int drawingFlags, uint32_t sdlTicks) {}

};

#endif
