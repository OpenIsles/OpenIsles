#include <cassert>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "gui/components/GuiGoodsSlotElement.h"
#include "gui/panel-widgets/GuiSelectedProductionBuildingWidget.h"
#include "map/Map.h"


GuiSelectedProductionBuildingWidget::GuiSelectedProductionBuildingWidget(const Context* const context) :
    GuiPanelWidget(context), guiProductionSlotsElement(context) {

    guiProductionSlotsElement.setPosition(15, 60);
    addChildElement(&guiProductionSlotsElement);

    goodsSlotInput = guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::INPUT);
    goodsSlotInput2 = guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::INPUT2);
    goodsSlotOutput = guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::OUTPUT);

    goodsSlotInput->setDisplayValue(true);
    goodsSlotInput2->setDisplayValue(true);
    goodsSlotOutput->setDisplayValue(true);
    goodsSlotOutput->setStatusBarText("Abholfertige Waren in diesem Gebäude");

    // TODO Child-Buttons für Stilllegen und "Abholung verbieten"
}

void GuiSelectedProductionBuildingWidget::renderElement(IRenderer* renderer) {
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
            goodsSlotInput, mapObjectType->inputAmountForProduction,
            mapObjectType->secondsToProduce, selectedBuilding->productionSlots.input.inventory,
            mapObjectType->buildingProduction.input.good->label,
            mapObjectType->buildingProduction.output.good->label);
    }
    if (goodsSlotInput2->isVisible()) {
        updateInputSlotStatusBarText(
            goodsSlotInput2, mapObjectType->input2AmountForProduction,
            mapObjectType->secondsToProduce, selectedBuilding->productionSlots.input2.inventory,
            mapObjectType->buildingProduction.input2.good->label,
            mapObjectType->buildingProduction.output.good->label);
    }

    // TODO Auslastung
    // TODO Betriebskosten
}

void GuiSelectedProductionBuildingWidget::updateInputSlotStatusBarText(
    GuiGoodsSlotElement* goodsSlotElement,
    double inputAmountForProduction, double secondsToProduce, double inputInventory,
    const std::string& inputLabel, const std::string& outputLabel) {

    // Ausrechnen, für wie viel Output-Güter die Input-Güter reichen
    double inputEnoughForXOutputGoods = inputInventory / inputAmountForProduction;
    char enoughForGoodsString[10];
    sprintf(enoughForGoodsString, "%.1f", inputEnoughForXOutputGoods);

    std::string statusBarText =
        inputLabel + "vorrat (reicht für weitere " + enoughForGoodsString + "t " + outputLabel + ")";
    goodsSlotElement->setStatusBarText(statusBarText);
}

void GuiSelectedProductionBuildingWidget::onSelectedMapBuildingChanged(const Building* newSelectedBuilding) {
    assert(newSelectedBuilding != nullptr);

    const ProductionSlots& productionSlots = newSelectedBuilding->productionSlots;
    guiProductionSlotsElement.setFromProductionSlots(productionSlots);
}