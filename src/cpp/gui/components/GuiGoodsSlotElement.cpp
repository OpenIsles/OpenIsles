#include <cassert>
#include "gui/components/GuiGoodsSlotElement.h"


GuiGoodsSlotElement::GuiGoodsSlotElement(const Context& context) : GuiGoodElement(context) {
}

GuiGoodsSlotElement::~GuiGoodsSlotElement() {
}

void GuiGoodsSlotElement::setGoodsSlot(const GoodsSlot* goodsSlot) {
    this->goodsSlot = goodsSlot;

    setGood(goodsSlot->good);
}

void GuiGoodsSlotElement::renderElement(IRenderer* renderer) {
    // Werte aktualisieren
    if (goodsSlot->isUsed()) {
        setValue(goodsSlot->inventory);
        setBarValue((double) goodsSlot->inventory / (double) goodsSlot->capacity);
    } else {
        setValue(0);
        setBarValue(0);
    }

    // Den Rest macht die Basisklasse
    GuiGoodElement::renderElement(renderer);
}
