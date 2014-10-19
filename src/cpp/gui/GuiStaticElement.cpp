#include "graphics/PlainGraphic.h"
#include "gui/GuiStaticElement.h"


GuiStaticElement::GuiStaticElement() {
}

GuiStaticElement::~GuiStaticElement() {
}

void GuiStaticElement::renderElement(SDL_Renderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);
    
    if (graphic != nullptr) {
        graphic->drawAt(windowX, windowY);
    }
}
