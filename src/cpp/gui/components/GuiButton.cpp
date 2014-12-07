#include <SDL.h>
#include "graphics/PlainGraphic.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiButton.h"


GuiButton::GuiButton(const Context* const context) : GuiStaticElement(context) {
}

GuiButton::~GuiButton() {
}

void GuiButton::renderElement(SDL_Renderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);
    
    PlainGraphic* graphicToUse = (pressed) ? graphicPressed : graphic;
    graphicToUse->drawAt(windowX, windowY);
}

void GuiButton::onEventElement(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && hitTest(event.button.x, event.button.y)) {
        pressed = true;
    }
    
    if (event.type == SDL_MOUSEBUTTONUP && pressed) {
        pressed = false;
        onClickFunction();
    }
}
