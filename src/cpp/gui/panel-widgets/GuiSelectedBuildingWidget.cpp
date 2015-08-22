#include <cassert>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "gui/panel-widgets/GuiSelectedBuildingWidget.h"
#include "map/Map.h"


GuiSelectedBuildingWidget::GuiSelectedBuildingWidget(const Context* const context) : GuiPanelWidget(context) {
    goodsSlotInput = new GuiGoodsSlotElement(context);
    goodsSlotInput->setDisplayValue(true);

    goodsSlotInput2 = new GuiGoodsSlotElement(context);
    goodsSlotInput2->setDisplayValue(true);

    goodsSlotOutput = new GuiGoodsSlotElement(context);
    goodsSlotOutput->setDisplayValue(true);
    goodsSlotOutput->setStatusBarText("Abholfertige Waren in diesem Gebäude");

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

    // TODO Child-Buttons für Stilllegen und "Abholung verbieten"
}

void GuiSelectedBuildingWidget::renderElement(IRenderer* renderer) {
    const Building* selectedBuilding =
        reinterpret_cast<const Building*>(context->game->getMap()->getSelectedMapObject());
    assert(selectedBuilding != nullptr);

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    const MapObjectType* mapObjectType = selectedBuilding->getMapObjectType();

    // Gebäudename
    context->guiMgr->drawPanelHeader(windowX, windowY, mapObjectType->title, nullptr);

    // produzierte Waren: Statusleisten-Texte bei Bedarf anpassen
    if (goodsSlotInput->isVisible()) {
        updateInputSlotStatusBarText(
            goodsSlotInput, mapObjectType->buildingProduction.input.rate,
            mapObjectType->buildingProduction.output.rate, selectedBuilding->productionSlots.input.inventory,
            mapObjectType->buildingProduction.input.good->label, mapObjectType->buildingProduction.output.good->label);
    }
    if (goodsSlotInput2->isVisible()) {
        updateInputSlotStatusBarText(
            goodsSlotInput2, mapObjectType->buildingProduction.input2.rate,
            mapObjectType->buildingProduction.output.rate, selectedBuilding->productionSlots.input2.inventory,
            mapObjectType->buildingProduction.input2.good->label, mapObjectType->buildingProduction.output.good->label);
    }

    // TODO Gebäude, die nix produzieren, müssen auch was anzeigen (öffentliche Gebäude). Aktuell sind nur Produktionsgebäude berücksichtigt

    // TODO Auslastung
    // TODO Betriebskosten
}

void GuiSelectedBuildingWidget::updateInputSlotStatusBarText(
    GuiGoodsSlotElement* goodsSlotElement, double inputRate, double outputRate, double inputInventory,
    const std::string& inputLabel, const std::string& outputLabel) {

    // Ausrechnen, für wie viel Output-Güter die Input-Güter reichen
    double productionRate = outputRate / inputRate;
    double input2EnoughForXOutputGoods = productionRate * inputInventory;
    char enoughForGoodsString[10];
    sprintf(enoughForGoodsString, "%.1f", input2EnoughForXOutputGoods);

    std::string statusBarText =
        inputLabel + "vorrat (reicht für weitere " + enoughForGoodsString + "t " + outputLabel + ")";
    goodsSlotElement->setStatusBarText(statusBarText);
}

void GuiSelectedBuildingWidget::onSelectedMapBuildingChanged(const Building* newSelectedBuilding) {
    assert(newSelectedBuilding != nullptr);

    const ProductionSlots* productionSlots = &newSelectedBuilding->productionSlots;
    const int productionY = 58;

    // TODO Gebäude, die nix produzieren, müssen auch was anzeigen (öffentliche Gebäude). Aktuell sind nur Produktionsgebäude berücksichtigt

    // Positionen und Sichtbarkeit anpassen. Zusätzlich den goodsSlot verzeigern.

    if (productionSlots->input2.isUsed()) {
        // input + input2 -> output

        goodsSlotInput->setPosition(42, productionY);
        goodsSlotInput->setGoodsSlot(&productionSlots->input);
        goodsSlotInput->setVisible(true);

        productionPlus->setPosition(88, productionY);
        productionPlus->setVisible(true);

        goodsSlotInput2->setPosition(103, productionY);
        goodsSlotInput2->setGoodsSlot(&productionSlots->input2);
        goodsSlotInput2->setVisible(true);

        productionArrow->setPosition(149, productionY);
        productionArrow->setVisible(true);

        goodsSlotOutput->setPosition(163, productionY);
        goodsSlotOutput->setGoodsSlot(&productionSlots->output);
        goodsSlotOutput->setVisible(true);
    }
    else if (productionSlots->input.isUsed()) {
        // input -> output

        goodsSlotInput->setPosition(73, productionY);
        goodsSlotInput->setGoodsSlot(&productionSlots->input);
        goodsSlotInput->setVisible(true);

        productionPlus->setVisible(false);
        goodsSlotInput2->setVisible(false);

        productionArrow->setPosition(119, productionY);
        productionArrow->setVisible(true);

        goodsSlotOutput->setPosition(133, productionY);
        goodsSlotOutput->setGoodsSlot(&productionSlots->output);
        goodsSlotOutput->setVisible(true);
    }
    else if (productionSlots->output.isUsed()) {
        // output

        goodsSlotInput->setVisible(false);
        goodsSlotInput2->setVisible(false);
        productionPlus->setVisible(false);
        productionArrow->setVisible(false);

        goodsSlotOutput->setPosition(103, productionY);
        goodsSlotOutput->setGoodsSlot(&productionSlots->output);
        goodsSlotOutput->setVisible(true);
    }
    else {
        // öffentliche Gebäude; hat gar nix

        goodsSlotInput->setVisible(false);
        goodsSlotInput2->setVisible(false);
        goodsSlotOutput->setVisible(false);
        productionPlus->setVisible(false);
        productionArrow->setVisible(false);
    }
}