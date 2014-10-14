#include "SDL.h"
#include "graphics/Graphic.h"
#include "gui/GuiButton.h"
#include "gui/GuiMgr.h"


GuiButton::GuiButton() {
}

GuiButton::~GuiButton() {
}

void GuiButton::renderElement(SDL_Renderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);
    
    Graphic* graphicToUse = (pressed) ? graphicPressed : graphic;
    
    SDL_Rect rectDestination = { windowX, windowY, width, height };
    SDL_RenderCopy(renderer, graphicToUse->getTexture(), nullptr, &rectDestination);
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
