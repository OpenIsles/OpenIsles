#include "SDL.h"
#include "graphics/PlainGraphic.h"
#include "gui/GuiMgr.h"
#include "gui/GuiPushButton.h"


GuiPushButton::GuiPushButton() {
}

GuiPushButton::~GuiPushButton() {
    delete graphicPressed;
}

void GuiPushButton::renderElement(SDL_Renderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    PlainGraphic* graphicToUse = (pressed) ? graphicPressed : (active ? graphicPressed : graphic);
    graphicToUse->drawAt(windowX, windowY);
}

void GuiPushButton::onEventElement(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && hitTest(event.button.x, event.button.y)) {
        pressed = true;
    }
    
    else if (event.type == SDL_MOUSEBUTTONUP && pressed) {
        pressed = false;
        if (hitTest(event.button.x, event.button.y)) {
            active = !active;
            onClickFunction();
        }
    }
}