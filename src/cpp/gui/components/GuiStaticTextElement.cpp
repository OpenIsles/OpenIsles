#include <cassert>
#include "graphics/mgr/IFontMgr.h"
#include "gui/components/GuiStaticTextElement.h"


GuiStaticTextElement::GuiStaticTextElement(const Context& context) : GuiBase(context) {
    shadowColor = nullptr;
}

GuiStaticTextElement::~GuiStaticTextElement() {
}

void GuiStaticTextElement::renderElement(IRenderer* renderer) {
    if (text.empty()) {
        return;
    }

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Position des Texts zum Element ausrechnen
    int x, y;

    if (align & RENDERTEXT_HALIGN_CENTER) {
        x = windowX + (width / 2);
    } else if (align & RENDERTEXT_HALIGN_RIGHT) {
        x = windowX + width;
    } else { // RENDERTEXT_HALIGN_LEFT
        x = windowX;
    }

    if (align & RENDERTEXT_VALIGN_MIDDLE) {
        y = windowY + (height / 2);
    } else if (align & RENDERTEXT_VALIGN_BOTTOM) {
        y = windowY + height;
    } else { // RENDERTEXT_VALIGN_TOP
        y = windowY;
    }

    context.fontMgr->renderText(renderer, text, x, y, color, shadowColor, fontName, fontSize, align);
}
