#include "graphics/mgr/IFontMgr.h"
#include "gui/panel-widgets/GuiDummyWidget.h"
#include "utils/Color.h"


static Color colorRed = Color(255, 0, 0, 255);
static Color colorYellow = Color(255, 255, 0, 255);


GuiDummyWidget::GuiDummyWidget(const Context* const context) : GuiPanelWidget(context) {
}

void GuiDummyWidget::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    context->fontMgr->renderText(renderer, "Dummy-Widget", windowX + width/2, windowY + height/2,
        &colorYellow, &colorRed, "DroidSans.ttf", 15, RENDERTEXT_HALIGN_CENTER | RENDERTEXT_VALIGN_MIDDLE);
}
