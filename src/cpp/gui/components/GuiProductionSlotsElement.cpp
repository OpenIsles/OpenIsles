#include <cassert>
#include "graphics/mgr/IGraphicsMgr.h"
#include "gui/components/GuiProductionSlotsElement.h"


GuiProductionSlotsElement::GuiProductionSlotsElement(const Context* const context) : GuiStaticElement(context) {
    // Größe ist fix
    width = 206;
    height = 42;

    goodsSlotInput = new GuiGoodsSlotElement(context);
    goodsSlotInput2 = new GuiGoodsSlotElement(context);
    goodsSlotOutput = new GuiGoodsSlotElement(context);

    productionPlus = new GuiStaticElement(context);
    productionPlus->setGraphic(
        context->graphicsMgr->getGraphicSet("production-plus")->getStatic()->getGraphic());

    productionArrow = new GuiStaticElement(context);
    productionArrow->setGraphic(
        context->graphicsMgr->getGraphicSet("production-arrow")->getStatic()->getGraphic());

    addChildElement(goodsSlotInput);
    addChildElement(goodsSlotInput2);
    addChildElement(goodsSlotOutput);
    addChildElement(productionPlus);
    addChildElement(productionArrow);
}

GuiProductionSlotsElement::~GuiProductionSlotsElement() {
    childElements.clear();

    delete goodsSlotInput;
    delete goodsSlotInput2;
    delete goodsSlotOutput;
    delete productionPlus;
    delete productionArrow;
}

void GuiProductionSlotsElement::setProductionSlots(unsigned char productionsSlotsToShow) {
    assert(
        (productionsSlotsToShow == 0) ||
        (productionsSlotsToShow == (ProductionSlot::OUTPUT)) ||
        (productionsSlotsToShow == (ProductionSlot::INPUT | ProductionSlot::OUTPUT)) ||
        (productionsSlotsToShow == (ProductionSlot::INPUT | ProductionSlot::INPUT2 | ProductionSlot::OUTPUT))
    );

    if (productionsSlotsToShow == (ProductionSlot::INPUT | ProductionSlot::INPUT2 | ProductionSlot::OUTPUT)) {
        // input + input2 -> output

        goodsSlotInput->setPosition(17, 0);
        goodsSlotInput->setVisible(true);

        productionPlus->setPosition(65, 0);
        productionPlus->setVisible(true);

        goodsSlotInput2->setPosition(82, 0);
        goodsSlotInput2->setVisible(true);

        productionArrow->setPosition(131, 0);
        productionArrow->setVisible(true);

        goodsSlotOutput->setPosition(147, 0);
        goodsSlotOutput->setVisible(true);
    }
    else if (productionsSlotsToShow == (ProductionSlot::INPUT | ProductionSlot::OUTPUT)) {
        // input -> output

        goodsSlotInput->setPosition(49, 0);
        goodsSlotInput->setVisible(true);

        productionPlus->setVisible(false);
        goodsSlotInput2->setVisible(false);

        productionArrow->setPosition(98, 0);
        productionArrow->setVisible(true);

        goodsSlotOutput->setPosition(115, 0);
        goodsSlotOutput->setVisible(true);
    }
    else if (productionsSlotsToShow == (ProductionSlot::OUTPUT)) {
        // output

        goodsSlotInput->setVisible(false);
        goodsSlotInput2->setVisible(false);
        productionPlus->setVisible(false);
        productionArrow->setVisible(false);

        goodsSlotOutput->setPosition(82, 0);
        goodsSlotOutput->setVisible(true);
    }
    else if (productionsSlotsToShow == 0) {
        goodsSlotInput->setVisible(false);
        goodsSlotInput2->setVisible(false);
        productionPlus->setVisible(false);
        productionArrow->setVisible(false);
        goodsSlotOutput->setVisible(false);
    }
    else {
        assert(false);
    }
}

GuiGoodsSlotElement* GuiProductionSlotsElement::getGoodsSlotElement(const ProductionSlot& productionSlot) {
    if (productionSlot == ProductionSlot::INPUT) {
        return goodsSlotInput;
    } else if (productionSlot == ProductionSlot::INPUT2) {
        return goodsSlotInput2;
    } else if (productionSlot == ProductionSlot::OUTPUT) {
        return goodsSlotOutput;
    } else {
        assert(false);
        return nullptr;
    }
}