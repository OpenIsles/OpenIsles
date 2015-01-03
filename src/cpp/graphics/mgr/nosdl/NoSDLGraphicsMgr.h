#ifndef _NOSDL_GRAPHICS_MGR_H
#define _NOSDL_GRAPHICS_MGR_H

#include "graphics/mgr/AbstractGraphicsMgr.h"
#include "graphics/graphic/nosdl/NoSDLGraphic.h"


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
	virtual IGraphic* loadGraphic(const char* filename) {
        return new NoSDLGraphic(renderer, filename, 0, 0);
    }

    virtual IGraphic* loadGraphic(const char* filename, unsigned char mapWidth, unsigned char mapHeight) {
        return new NoSDLGraphic(renderer, filename, mapWidth, mapHeight);
    }

    virtual IGraphic* loadGraphic(
        const IGraphic& srcGraphic, const Rect& srcRect, unsigned char mapWidth, unsigned char mapHeight) {

        return new NoSDLGraphic(renderer, srcGraphic, srcRect, mapWidth, mapHeight);
    }

};

#endif
