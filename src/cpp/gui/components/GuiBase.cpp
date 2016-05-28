#include "gui/components/GuiBase.h"
#include "gui/GuiMgr.h"


bool GuiBase::hitTest(int windowX, int windowY) const {
    // Unsichtbare Elemente werden nicht getroffen
    if (!visible) {
        return false;
    }

    // Gibt es sichtbare Kinder an dieser Stelle? Dann liegen sie drüber und wir treffen nicht hier
    for (const GuiBase* childElement : childElements) {
        if (childElement->hitTest(windowX, windowY)) {
            return false;
        }
    }

    // Fensterkoordinaten ermitteln
    int thisWindowX, thisWindowY;
    getWindowCoords(thisWindowX, thisWindowY);

    // Kollision prüfen
    return ((windowX >= thisWindowX) && (windowY >= thisWindowY) &&
            (windowX < thisWindowX + width) && (windowY < thisWindowY + height));
}

bool GuiBase::onEvent(SDL_Event& event) {
    // Unsichtbare Elemente kriegen keine Events
    if (!visible) {
        return true;
    }

    // Erst den allgemeinen Handler aufrufen
    if (!onEventElement(event)) {
        return false;
    }

    // Mausbewegung? Separate Event-Handler aufrufen
    if (event.type == SDL_MOUSEMOTION) {
        int x = event.motion.x;
        int y = event.motion.y;

        bool hit = hitTest(x, y);
        if (hit) {
            if (!onMouseMove(event.motion)) {
                return false;
            }
        }

        // Enter/Leave?
        int oldX = x - event.motion.xrel;
        int oldY = y - event.motion.yrel;
        bool oldHit = hitTest(oldX, oldY);

        if (hit && !oldHit) {
            hovered = true;
            context.guiMgr->setStatusBarText(statusBarText);

            if (!onMouseEnter(event.motion)) {
                return false;
            }
        }
        else if (!hit && oldHit) {
            hovered = false;

            if (!onMouseLeave(event.motion)) {
                return false;
            }
        }
    }

    // Jetzt an die Kinder weitergeben
    for (auto iter = childElements.cbegin(); iter != childElements.cend(); iter++) {
        if (!(*iter)->onEvent(event)) {
            return false;
        }
    }
    return true;
}

void GuiBase::setStatusBarText(const std::string& statusBarText) {
    this->statusBarText = statusBarText;

    // Wurde der Text geändert während der Mauszeiger grade über dem Button steht,
    // müssen wir die Statuszeile sofort ändern.
    if (hovered) {
        context.guiMgr->setStatusBarText(statusBarText);
    }
}