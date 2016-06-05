#ifndef _SDL_GRAPHICS_MGR_H
#define _SDL_GRAPHICS_MGR_H

#include "graphics/graphic/sdl/SDLGraphic.h"
#include "graphics/mgr/AbstractGraphicsMgr.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "graphics/renderer/IRenderer.h"


/**
 * @brief Grafik-Manager für SDL-Grafiken
 */
class SDLGraphicsMgr : public AbstractGraphicsMgr {

public:
    /**
     * @brief Lädt alle Grafiken für das Spiel
     * @param renderer (Dependency)
     * @param configMgr (Dependency)
     */
    SDLGraphicsMgr(IRenderer* const renderer, ConfigMgr* const configMgr);
    virtual ~SDLGraphicsMgr() {}

private:
    virtual IGraphic* loadGraphic(const char* filename) {
        return new SDLGraphic(renderer, filename);
    }

    virtual IGraphic* loadGraphic(const IGraphic& srcGraphic, const Rect& srcRect) {
        return new SDLGraphic(renderer, dynamic_cast<const SDLGraphic&>(srcGraphic), srcRect);
    }

};

#endif
