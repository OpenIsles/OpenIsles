#include "graphics/mgr/IFontMgr.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "gui/components/GuiStatusBar.h"
#include "utils/Color.h"


GuiStatusBar::GuiStatusBar(const Context& context) : GuiStaticGraphicElement(context), textElement(context) {
    const IGraphic* backgroundGraphic = context.graphicsMgr->getGraphicSet("statusbar")->getStatic()->getGraphic();
    setPosition(0, 734);
    setGraphic(backgroundGraphic);

    constexpr int paddingX = 15;
    textElement.setCoords(paddingX, 0, backgroundGraphic->getWidth() - 2*paddingX, backgroundGraphic->getHeight());
    textElement.setColor(&Color::white);
    textElement.setFontName("DroidSans-Bold.ttf");
    textElement.setFontSize(14);
    textElement.setAlign(RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_MIDDLE);

    addChildElement(&textElement);
}

GuiStatusBar::~GuiStatusBar() {
}

void GuiStatusBar::setText(const std::string& text) {
    textElement.setText(text);
}