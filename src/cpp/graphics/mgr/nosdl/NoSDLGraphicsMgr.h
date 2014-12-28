#ifndef _NOSDL_GRAPHICS_MGR_H
#define _NOSDL_GRAPHICS_MGR_H

#include "graphics/mgr/AbstractGraphicsMgr.h"
#include "graphics/graphic/nosdl/NoSDLAnimation.h"
#include "graphics/graphic/nosdl/NoSDLMapObjectGraphic.h"
#include "graphics/graphic/nosdl/NoSDLPlainGraphic.h"


/**
 * @brief Grafik-Manager für die Tests
 */
class NoSDLGraphicsMgr : public AbstractGraphicsMgr {

public:
    /**
     * @brief Lädt alle Grafiken für das Spiel
     * @param renderer (Dependency)
     * @param configMgr (Dependency)
     */
    NoSDLGraphicsMgr(IRenderer* const renderer, const ConfigMgr* const configMgr) :
        AbstractGraphicsMgr(renderer, configMgr) {}

    virtual ~NoSDLGraphicsMgr() {}

private:
	virtual IPlainGraphic* loadPlainGraphic(const char* filename) {
        return new NoSDLPlainGraphic(renderer, filename);
    }

    virtual IMapObjectGraphic* loadMapObjectGraphic(
        const char* filename, unsigned char mapWidth, unsigned char mapHeight) {

        return new NoSDLMapObjectGraphic(renderer, filename, mapWidth, mapHeight);
    }

    virtual IAnimation* loadAnimation(const char* filename, unsigned char mapWidth, unsigned char mapHeight,
                                      unsigned int framesCount, double fps) {

        return new NoSDLAnimation(renderer, filename, mapWidth, mapHeight, framesCount, fps);
    }

};

#endif
