#include "graphics/mgr/IFontMgr.h"
#include "gui/components/GuiIconWithStringElement.h"
#include "utils/Color.h"


static Color colorBlack = Color(0, 0, 0, 255);
static Color colorWhite = Color(255, 255, 255, 255);


void GuiIconWithStringElement::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    int yMiddle = windowY + (height / 2);

    graphic->drawAt(windowX, yMiddle - graphic->getHeight() / 2);

    const int fontSize = 14;
    context->fontMgr->renderText(
        renderer, string, windowX + graphic->getWidth() + 4, yMiddle,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", fontSize, RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);
}