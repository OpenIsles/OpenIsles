#include <SDL.h>
#include "graphics/graphic/IGraphic.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiButton.h"


GuiButton::GuiButton(const Context* const context) : GuiStaticGraphicElement(context) {
}

GuiButton::~GuiButton() {
}

void GuiButton::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);
    
    const IGraphic* graphicToUse = (pressed) ? graphicPressed : graphic;
    graphicToUse->drawAt(windowX, windowY);
}

bool GuiButton::onEventElement(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT &&
        hitTest(event.button.x, event.button.y)) {

        pressed = true;
        return false;
    }
    
    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT &&
        pressed) {

        pressed = false;
        onClickFunction();
        return false;
    }

    return true;
}

