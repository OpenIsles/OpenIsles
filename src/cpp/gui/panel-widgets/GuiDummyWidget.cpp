#include "defines.h"
#include "game/Game.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/panel-widgets/GuiDummyWidget.h"
#include "utils/Color.h"


static Color colorRed = Color(255, 0, 0, 255);
static Color colorYellow = Color(255, 255, 0, 255);


GuiDummyWidget::GuiDummyWidget(const Context* const context) : GuiPanelWidget(context) {
    panelHeader = new GuiPanelHeader(context);
    panelHeader->setPosition(0, 0);
    panelHeader->setText(_("Dummy widget"));
    addChildElement(panelHeader);
}

GuiDummyWidget::~GuiDummyWidget() {
    childElements.clear();

    delete panelHeader;
}

void GuiDummyWidget::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    context->fontMgr->renderText(renderer, _("Dummy widget"), windowX + width/2, windowY + height/2,
        &colorYellow, &colorRed, "DroidSans.ttf", 15, RENDERTEXT_HALIGN_CENTER | RENDERTEXT_VALIGN_MIDDLE);
}