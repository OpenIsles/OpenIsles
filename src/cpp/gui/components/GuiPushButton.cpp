#include <SDL.h>
#include "graphics/graphic/IPlainGraphic.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiPushButton.h"


GuiPushButton::GuiPushButton(const Context* const context) : GuiButton(context) {
}

GuiPushButton::~GuiPushButton() {
}

void GuiPushButton::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    IPlainGraphic* graphicToUse = (pressed) ? graphicPressed : (active ? graphicPressed : graphic);
    graphicToUse->drawAt(windowX, windowY);
}

void GuiPushButton::onEventElement(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT &&
        hitTest(event.button.x, event.button.y)) {

        pressed = true;
    }
    
    else if (event.type == SDL_MOUSEBUTTONUP &&
             event.button.button == SDL_BUTTON_LEFT &&
             pressed) {

        pressed = false;
        if (hitTest(event.button.x, event.button.y)) {
            active = !active;
            onClickFunction();
        }
    }
}