#include "SDL.h"
#include "GuiButton.h"
#include "GuiStaticElement.h"
#include "Graphic.h"


GuiButton::GuiButton() {
}

GuiButton::~GuiButton() {
}

void GuiButton::render(SDL_Renderer* renderer) {
    Graphic* graphicToUse = (pressed) ? graphicPressed : graphic;
    
    SDL_Rect rectDestination = { windowX, windowY, graphicToUse->getWidth(), graphicToUse->getHeight() };
    SDL_RenderCopy(renderer, graphicToUse->getTexture(), nullptr, &rectDestination);
}

void GuiButton::onEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.x >= windowX && event.button.y >= windowY && 
                event.button.x < windowX + graphic->getWidth() &&
                event.button.y < windowY + graphic->getHeight()) {
            
            pressed = !pressed;
        }
    }
}
