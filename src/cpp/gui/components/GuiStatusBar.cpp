#include "graphics/mgr/IFontMgr.h"
#include "gui/components/GuiStatusBar.h"
#include "utils/Color.h"

static Color colorWhite = Color(255, 255, 255, 255);


GuiStatusBar::GuiStatusBar(const Context* const context) : GuiStaticElement(context) {
}

GuiStatusBar::~GuiStatusBar() {
}

void GuiStatusBar::renderElement(IRenderer* renderer) {
    GuiStaticElement::renderElement(renderer);

    if (text.empty()) {
        return;
    }

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    context->fontMgr->renderText(renderer, text, windowX + width - 15, windowY + (height / 2), &colorWhite, nullptr,
                                 "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_MIDDLE);
}