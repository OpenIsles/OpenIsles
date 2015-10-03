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
        return new NoSDLGraphic(renderer, filename);
    }

    virtual IGraphic* loadGraphic(const IGraphic& srcGraphic, const Rect& srcRect) {
        return new NoSDLGraphic(renderer, srcGraphic, srcRect);
    }

};

#endif
