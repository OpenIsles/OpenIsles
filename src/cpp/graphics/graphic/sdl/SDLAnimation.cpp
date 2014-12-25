#include <stdexcept>
#include <iostream>
#include "graphics/graphic/sdl/SDLAnimation.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "utils/Rect.h"


SDLAnimation::SDLAnimation(IRenderer* const renderer, const char* filename,
                     unsigned char mapWidth, unsigned char mapHeight,
                     unsigned int framesCount, double fps) :
    SDLMapObjectGraphic(renderer, filename, mapWidth, mapHeight), framesCount(framesCount), fps(fps)
{

    // Prüfen, ob die Grafik ordentlich ist. Die Breite muss exakt ein Vielfaches der Framesanzahl sein
    if (width % framesCount != 0) {
        std::cerr << "Could not load animation '" << filename << "': frames do not match" << std::endl;
        throw new std::runtime_error("Could not load animation");
    }

    frameWidth = width / framesCount;
}

void SDLAnimation::drawFrameScaledAt(int x, int y, double scale, unsigned int frame) {
    if (frame < 0 || frame >= framesCount) {
        std::cerr << "Illegal frame " << frame << " for animation '" << filename << "'" << std::endl;
        throw new std::runtime_error("Illegal frame");
    }

    SDL_Rect rectSource = { (int)frame * frameWidth, 0, frameWidth, height };
    SDL_Rect rectDestination = { x, y, (int) (frameWidth * scale), (int) (height * scale) };
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_RenderCopy(sdlRealRenderer, texture, &rectSource, &rectDestination);
}