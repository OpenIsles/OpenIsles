#include "gui/components/GuiGoodsSlotElement.h"
#include "graphics/mgr/IFontMgr.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "utils/Color.h"


static Color colorWhite = Color(255, 255, 255, 255);
static Color colorBlack = Color(0, 0, 0, 255);


GuiGoodsSlotElement::GuiGoodsSlotElement(const Context* const context) : GuiStaticElement(context) {
    // Größe ist fix
    width = 32;
    height = 32;
}

GuiGoodsSlotElement::~GuiGoodsSlotElement() {
}

void GuiGoodsSlotElement::setGoodsSlot(const GoodsSlot* goodsSlot) {
    this->goodsSlot = goodsSlot;

    // Grafik aktualisieren
    const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(goodsSlot->good, true);
    setGraphic(context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic());
}

void GuiGoodsSlotElement::renderElement(IRenderer* renderer) {
    GuiStaticElement::renderElement(renderer);

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    char inventoryOutput[10];
    sprintf(inventoryOutput, "%.0ft", floor(goodsSlot->inventory));
    context->fontMgr->renderText(renderer, inventoryOutput, windowX + 40, windowY + 42, &colorWhite, &colorBlack,
        "DroidSans.ttf", 12, RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_BOTTOM);
}
