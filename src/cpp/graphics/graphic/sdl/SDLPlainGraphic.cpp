#include "graphics/graphic/sdl/SDLPlainGraphic.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "utils/Rect.h"


SDLPlainGraphic::SDLPlainGraphic(IRenderer* const renderer, const char* filename) : SDLGraphic(renderer, filename) {
}

SDLPlainGraphic::~SDLPlainGraphic() {
}

void SDLPlainGraphic::drawAt(int x, int y) {
    SDL_Rect rectDestination = { x, y, width, height };
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_RenderCopy(sdlRealRenderer, texture, nullptr, &rectDestination);
}

void SDLPlainGraphic::drawScaledAt(int x, int y, double scale) {
    SDL_Rect rectDestination = { x, y, (int) (getWidth() * scale), (int) (getHeight() * scale) };
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_RenderCopy(sdlRealRenderer, texture, nullptr, &rectDestination);
}