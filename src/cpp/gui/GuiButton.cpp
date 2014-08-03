#include <iostream>
#include "SDL.h"
#include "gui/GuiButton.h"
#include "gui/GuiMgr.h"
#include "gui/GuiStaticElement.h"
#include "Graphic.h"


GuiButton::GuiButton() {
}

GuiButton::~GuiButton() {
}

void GuiButton::render(SDL_Renderer* renderer) {
    Graphic* graphicToUse = (pressed) ? graphicPressed : graphic;
    
    SDL_Rect rectDestination = { windowX, windowY, width, height };
    SDL_RenderCopy(renderer, graphicToUse->getTexture(), nullptr, &rectDestination);
}

void GuiButton::onEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && hitTest(event.button.x, event.button.y)) {
        pressed = true;
    }
    
    if (event.type == SDL_MOUSEBUTTONUP && pressed) {
        pressed = false;
        
        // TODO was tun
        std::cout << "Button wurde gedrückt." << std::endl;
    }
}
