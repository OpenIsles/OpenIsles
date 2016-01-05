#include "graphics/mgr/IFontMgr.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "gui/components/GuiPanelHeader.h"
#include "utils/Color.h"

static Color colorBlack = Color(0, 0, 0, 255);
static Color colorLightBrown = Color(223, 216, 183, 255);


GuiPanelHeader::GuiPanelHeader(const Context& context) : GuiStaticGraphicElement(context) {
    // Größe ist fix
    width = 248;
    height = 40;

    setGraphic(context.graphicsMgr->getGraphicSet("panel-header")->getStatic()->getGraphic());

    textElement = new GuiStaticTextElement(context);
    textElement->setCoords(35, 10, 178, 18);
    textElement->setColor(&colorLightBrown);
    textElement->setShadowColor(&colorBlack);
    textElement->setFontName("DroidSans-Bold.ttf");
    textElement->setFontSize(14);
    textElement->setAlign(RENDERTEXT_HALIGN_CENTER | RENDERTEXT_VALIGN_MIDDLE);

    addChildElement(textElement);
}

GuiPanelHeader::~GuiPanelHeader() {
    childElements.clear();

    delete textElement;
}

void GuiPanelHeader::setText(const std::string& text) {
    textElement->setText(text);
}

void GuiPanelHeader::setCoatOfArmsIcon(const Player* player) {
    const std::string graphicSetName =
        context.graphicsMgr->getGraphicSetNameForCoatOfArms("large", player->getColor());
    const IGraphic* coatOfArmsGraphic =
        context.graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic();

    setIconGraphic(coatOfArmsGraphic);
}

void GuiPanelHeader::renderElement(IRenderer* renderer) {
    GuiStaticGraphicElement::renderElement(renderer);

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    if (iconGraphic != nullptr) {
        iconGraphic->drawShadowAt(windowX + 209, windowY + 6);
        iconGraphic->drawAt(windowX + 203, windowY);
    }
}
