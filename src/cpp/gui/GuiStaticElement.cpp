#include "SDL.h"
#include "GuiStaticElement.h"
#include "Graphic.h"


GuiStaticElement::GuiStaticElement() {
}

GuiStaticElement::~GuiStaticElement() {
}

void GuiStaticElement::render(SDL_Renderer* renderer) {
    SDL_Rect rectDestination = { windowX, windowY, graphic->getWidth(), graphic->getHeight() };
	SDL_RenderCopy(renderer, graphic->getTexture(), nullptr, &rectDestination);
}
