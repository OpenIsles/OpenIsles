#include <cassert>
#include "gui/components/GuiGoodsSlotElement.h"


GuiGoodsSlotElement::GuiGoodsSlotElement(const Context* const context) : GuiGoodElement(context) {
}

GuiGoodsSlotElement::~GuiGoodsSlotElement() {
}

void GuiGoodsSlotElement::setGoodsSlot(const GoodsSlot* goodsSlot) {
    this->goodsSlot = goodsSlot;

    setGood(goodsSlot->good);
}

void GuiGoodsSlotElement::renderElement(IRenderer* renderer) {
    assert(goodsSlot->capacity > 0);

    // Werte aktualisieren
    setValue(goodsSlot->inventory);
    setBarValue((double) goodsSlot->inventory / (double) goodsSlot->capacity);

    // Den Rest macht die Basisklasse
    GuiGoodElement::renderElement(renderer);
}
