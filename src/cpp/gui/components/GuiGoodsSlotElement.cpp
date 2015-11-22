#include <cassert>
#include <cmath>
#include <cstdio>
#include "gui/components/GuiGoodsSlotElement.h"
#include "graphics/mgr/IFontMgr.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "utils/Color.h"
#include "utils/Rect.h"


static Color colorWhite = Color(255, 255, 255, 255);
static Color colorBlack = Color(0, 0, 0, 255);
static Color colorRedBar = Color(255, 0, 0, 255);


GuiGoodsSlotElement::GuiGoodsSlotElement(const Context* const context) : GuiStaticGraphicElement(context) {
    // Größe ist fix
    width = 42;
    height = 42;
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
    GuiStaticGraphicElement::renderElement(renderer);

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Wert anzeigen
    if (displayValue) {
        char inventoryOutput[10];
        std::sprintf(inventoryOutput, "%.0ft", std::floor(goodsSlot->inventory));
        context->fontMgr->renderText(renderer, inventoryOutput, windowX + 40, windowY + 42, &colorWhite, &colorBlack,
            "DroidSans.ttf", 12, RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_BOTTOM);
    }

    // Balken anzeigen
    else if (displayBar) {
        assert(goodsSlot->capacity > 0);
        int barHeight = (int) (goodsSlot->inventory / goodsSlot->capacity * height);

        renderer->setDrawColor(colorRedBar);

        const int barWidth = 4;
        Rect rect(windowX + width - barWidth, windowY + (height - barHeight), barWidth, barHeight);
        renderer->fillRect(rect);
    }
}
