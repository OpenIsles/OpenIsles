#include <SDL.h>
#include "graphics/graphic/IGraphic.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiButton.h"


GuiButton::GuiButton(const Context& context) : GuiStaticGraphicElement(context) {
}

GuiButton::~GuiButton() {
}

void GuiButton::renderElement(IRenderer* renderer) {
    const IGraphic* graphicToUse = (pressed) ? graphicPressed : graphic;
    drawGraphic(graphicToUse);
}

void GuiButton::drawGraphic(const IGraphic* graphicToUse) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Schatten nutzt grundsätzlich graphic! Abgeleitete Klassen können dies nicht verändern, damit der
    // Schatten immer gleich bleibt (und sich z.B. nicht ändert, wenn der GuiPushButton in den active-Zustand geht)
    if (useShadow) {
        // TODO gucken, ob das ein bool bleiben kann, oder ob wir shadowX/Y brauchen, um unterschiedliche
        // Schattenweiten zu werfen, weil die Buttons nicht einheitlich sein können.
        graphic->drawShadowAt(windowX + 5, windowY + 14);
    }

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

