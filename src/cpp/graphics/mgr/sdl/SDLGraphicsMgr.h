#ifndef _SDL_GRAPHICS_MGR_H
#define _SDL_GRAPHICS_MGR_H

#include "graphics/graphic/sdl/SDLAnimation.h"
#include "graphics/graphic/sdl/SDLMapObjectGraphic.h"
#include "graphics/graphic/sdl/SDLPlainGraphic.h"
#include "graphics/mgr/AbstractGraphicsMgr.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "graphics/renderer/IRenderer.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"



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
    SDLGraphicsMgr(IRenderer* const renderer, const ConfigMgr* const configMgr);
    virtual ~SDLGraphicsMgr() {}

private:
	virtual IPlainGraphic* loadPlainGraphic(const char* filename) {
        return new SDLPlainGraphic(renderer, filename);
    }

    virtual IMapObjectGraphic* loadMapObjectGraphic(
        const char* filename, unsigned char mapWidth, unsigned char mapHeight) {

        return new SDLMapObjectGraphic(renderer, filename, mapWidth, mapHeight);
    }

    virtual IAnimation* loadAnimation(const char* filename, unsigned char mapWidth, unsigned char mapHeight,
                                      unsigned int framesCount, double fps) {

        return new SDLAnimation(renderer, filename, mapWidth, mapHeight, framesCount, fps);
    }

};

#endif
