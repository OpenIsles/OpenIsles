#include <cassert>
#include "graphics/mgr/IGraphicsMgr.h"
#include "gui/components/GuiProductionSlotsElement.h"


GuiProductionSlotsElement::GuiProductionSlotsElement(const Context& context) : GuiStaticGraphicElement(context) {
    // Größe ist fix
    width = 206;
    height = 42;

    goodsSlotInput = new GuiGoodsSlotElement(context);
    goodsSlotInput2 = new GuiGoodsSlotElement(context);
    goodsSlotOutput = new GuiGoodsSlotElement(context);

    productionPlus = new GuiStaticGraphicElement(context);
    productionPlus->setGraphic(
        context.graphicsMgr->getGraphicSet("production-plus")->getStatic()->getGraphic());

    productionArrow = new GuiStaticGraphicElement(context);
    productionArrow->setGraphic(
        context.graphicsMgr->getGraphicSet("production-arrow")->getStatic()->getGraphic());

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

void GuiProductionSlotsElement::setFromProductionSlots(const ProductionSlots& productionSlots) {
    // Gucken, was wir anzeigen. Unsichtbare Güter werden nicht gezeigt
    const GoodsSlot* inputGoodsSlotToShow = nullptr;
    const GoodsSlot* input2GoodsSlotToShow = nullptr;
    const GoodsSlot* outputGoodsSlotToShow = nullptr;

    if (productionSlots.input2.isUsed()) {
        if (!productionSlots.input.good->invisible && !productionSlots.input2.good->invisible) {
            inputGoodsSlotToShow = &productionSlots.input;
            input2GoodsSlotToShow = &productionSlots.input2;
        } else if (!productionSlots.input.good->invisible && productionSlots.input2.good->invisible) {
            inputGoodsSlotToShow = &productionSlots.input;
            input2GoodsSlotToShow = nullptr;
        } else if (productionSlots.input.good->invisible && !productionSlots.input2.good->invisible) {
            inputGoodsSlotToShow = &productionSlots.input2;
            input2GoodsSlotToShow = nullptr;
        }
    }
    else if (productionSlots.input.isUsed()) {
        if (!productionSlots.input.good->invisible) {
            inputGoodsSlotToShow = &productionSlots.input;
        }
    }

    if (productionSlots.output.isUsed() && !productionSlots.output.good->invisible) {
        outputGoodsSlotToShow = &productionSlots.output;
    }

    // TODO Gebäude, die nix produzieren, müssen auch was anzeigen (öffentliche Gebäude). Aktuell sind nur Produktionsgebäude berücksichtigt

    // Positionen und Sichtbarkeit anpassen.
    if (input2GoodsSlotToShow != nullptr) {
        setProductionSlots(
            ProductionSlot::INPUT | ProductionSlot::INPUT2 | ProductionSlot::OUTPUT);
    }
    else if (inputGoodsSlotToShow != nullptr) {
        setProductionSlots(ProductionSlot::INPUT | ProductionSlot::OUTPUT);
    }
    else if (outputGoodsSlotToShow != nullptr) {
        setProductionSlots(ProductionSlot::OUTPUT);
    }
    else {
        // öffentliche Gebäude; hat gar nix, also ausblenden
        setProductionSlots(0);
    }

    // goodsSlots verzeigern
    if (inputGoodsSlotToShow != nullptr) {
        goodsSlotInput->setGoodsSlot(inputGoodsSlotToShow);
    }
    if (input2GoodsSlotToShow != nullptr) {
        goodsSlotInput2->setGoodsSlot(input2GoodsSlotToShow);
    }
    if (outputGoodsSlotToShow != nullptr) {
        goodsSlotOutput->setGoodsSlot(outputGoodsSlotToShow);
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