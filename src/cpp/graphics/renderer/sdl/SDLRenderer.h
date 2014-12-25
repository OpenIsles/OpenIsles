#ifndef _SDL_RENDERER_H
#define _SDL_RENDERER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include "graphics/renderer/IRenderer.h"
#include "utils/Color.h"
#include "utils/Rect.h"

/**
 * @brief Interface für den Renderer, der alles übernimmt, was mit Zeichnen zu tun hat.
 * Damit wird es uns möglich, die Anwendung auch ohne SDL für die Tests zu kompilieren.
 */
class SDLRenderer : public IRenderer {

private:
    /**
     * SDL-Fenster
     */
    SDL_Window* window;

    /**
     * der SDL-Renderer
     */
    SDL_Renderer* renderer;

    /**
     * SDL-Surface für das Anwendungssymbol
     */
    SDL_Surface* surfaceAppIcon;

    /**
     * SDL-Texture für den Backbuffer, auf den wir alles zeichnen
     */
    SDL_Texture* offscreenTexture;

public:
    SDLRenderer();
    virtual ~SDLRenderer();

    virtual const int getWindowWidth();
    virtual const int getWindowHeight();

    virtual void setClipRect(const Rect* const rect);
    virtual void setDrawColor(const Color& color);
    virtual void setDrawBlendMode(int blendMode);
    virtual void drawLine(int x1, int y1, int x2, int y2);
    virtual void fillRect(const Rect& rect);
    virtual void startFrame();
    virtual void endFrame();

    /**
     * @brief Liefert den echten SDL-Renderer zurück.
     * @return SDL-Renderer, der in allen SDL-Funktionen benutzt werden kann
     */
    SDL_Renderer* getRealRenderer();
};

#endif
