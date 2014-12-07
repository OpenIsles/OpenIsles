#include "gui/components/GuiStaticElement.h"


GuiStaticElement::GuiStaticElement(const Context* const context) : GuiBase(context) {
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
