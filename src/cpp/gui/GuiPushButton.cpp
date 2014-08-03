#include "SDL.h"
#include "gui/GuiMgr.h"
#include "gui/GuiPushButton.h"
#include "gui/GuiStaticElement.h"
#include "Graphic.h"


GuiPushButton::GuiPushButton() {
}

GuiPushButton::~GuiPushButton() {
}

void GuiPushButton::render(SDL_Renderer* renderer) {
    Graphic* graphicToUse = (pressed) ? graphicPressed : (active ? graphicPressed : graphic);
    
    SDL_Rect rectDestination = { windowX, windowY, width, height };
    SDL_RenderCopy(renderer, graphicToUse->getTexture(), nullptr, &rectDestination);
}

void GuiPushButton::onEvent(SDL_Event& event) {
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