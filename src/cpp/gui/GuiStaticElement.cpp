#include "SDL.h"
#include "GuiStaticElement.h"
#include "Graphic.h"


GuiStaticElement::GuiStaticElement() {
}

GuiStaticElement::~GuiStaticElement() {
    delete graphic;
}

void GuiStaticElement::render(SDL_Renderer* renderer) {
    SDL_Rect rectDestination = { windowX, windowY, width, height };
	SDL_RenderCopy(renderer, graphic->getTexture(), nullptr, &rectDestination);
}
