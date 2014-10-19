#include <stdexcept>
#include <iostream>
#include "graphics/PlainGraphic.h"

extern SDL_Renderer* renderer;


PlainGraphic::PlainGraphic(const char* filename) : Graphic(filename) {
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