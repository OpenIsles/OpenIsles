#include "graphics/PlainGraphic.h"


PlainGraphic::PlainGraphic(SDL_Renderer* const renderer, const char* filename) : Graphic(renderer, filename) {
}

PlainGraphic::~PlainGraphic() {
}

void PlainGraphic::drawAt(int x, int y) {
    SDL_Rect rectDestination = { x, y, width, height };
    SDL_RenderCopy(renderer, texture, nullptr, &rectDestination);
}

void PlainGraphic::drawScaledAt(int x, int y, double scale) {
    SDL_Rect rectDestination = { x, y, (int) (getWidth() * scale), (int) (getHeight() * scale) };
    SDL_RenderCopy(renderer, texture, nullptr, &rectDestination);
}