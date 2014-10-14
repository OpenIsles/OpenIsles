#include "SDL.h"
#include "graphics/Graphic.h"
#include "gui/GuiStaticElement.h"


GuiStaticElement::GuiStaticElement() {
}

GuiStaticElement::~GuiStaticElement() {
}

void GuiStaticElement::renderElement(SDL_Renderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);
    
    if (graphic != nullptr) {
        SDL_Rect rectDestination = { windowX, windowY, width, height };
        SDL_RenderCopy(renderer, graphic->getTexture(), nullptr, &rectDestination);
    }
}
