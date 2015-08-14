#include <SDL.h>
#include "graphics/graphic/IGraphic.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiButton.h"


GuiButton::GuiButton(const Context* const context) : GuiStaticElement(context) {
}

GuiButton::~GuiButton() {
}

void GuiButton::setStatusBarText(const std::string& statusBarText) {
    this->statusBarText = statusBarText;

    // Wurde der Text geändert während der Mauszeiger grade über dem Button steht,
    // müssen wir die Statuszeile sofort ändern.
    if (hovered) {
        context->guiMgr->setStatusBarText(statusBarText);
    }
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

bool GuiButton::onMouseEnter(SDL_MouseMotionEvent& event) {
    hovered = true;
    context->guiMgr->setStatusBarText(statusBarText);
    return false;
}

bool GuiButton::onMouseLeave(SDL_MouseMotionEvent& event) {
    hovered = false;
    return false;
}
