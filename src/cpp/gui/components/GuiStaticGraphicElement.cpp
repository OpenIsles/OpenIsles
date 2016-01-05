#include "gui/components/GuiStaticGraphicElement.h"


GuiStaticGraphicElement::GuiStaticGraphicElement(const Context& context) : GuiBase(context) {
}

GuiStaticGraphicElement::~GuiStaticGraphicElement() {
}

void GuiStaticGraphicElement::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);
    
    if (graphic != nullptr) {
        graphic->drawAt(windowX, windowY);
    }
}
