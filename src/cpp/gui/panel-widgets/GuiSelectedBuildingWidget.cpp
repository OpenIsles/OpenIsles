#include <cassert>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "gui/components/GuiGoodsSlotElement.h"
#include "gui/panel-widgets/GuiSelectedBuildingWidget.h"
#include "map/Map.h"


GuiSelectedBuildingWidget::GuiSelectedBuildingWidget(const Context* const context) :
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
            mapObjectType->buildingProduction.input.good->label,
            mapObjectType->buildingProduction.output.good->label);
    }
    if (goodsSlotInput2->isVisible()) {
        updateInputSlotStatusBarText(
            goodsSlotInput2, mapObjectType->buildingProduction.input2.rate,
            mapObjectType->buildingProduction.output.rate, selectedBuilding->productionSlots.input2.inventory,
            mapObjectType->buildingProduction.input2.good->label,
            mapObjectType->buildingProduction.output.good->label);
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

    // TODO Gebäude, die nix produzieren, müssen auch was anzeigen (öffentliche Gebäude). Aktuell sind nur Produktionsgebäude berücksichtigt

    // Positionen und Sichtbarkeit anpassen.
    if (productionSlots->input2.isUsed()) {
        guiProductionSlotsElement.setProductionSlots(
            ProductionSlot::INPUT | ProductionSlot::INPUT2 | ProductionSlot::OUTPUT);
    }
    else if (productionSlots->input.isUsed()) {
        guiProductionSlotsElement.setProductionSlots(ProductionSlot::INPUT | ProductionSlot::OUTPUT);
    }
    else if (productionSlots->output.isUsed()) {
        guiProductionSlotsElement.setProductionSlots(ProductionSlot::OUTPUT);
    }
    else {
        // öffentliche Gebäude; hat gar nix, also ausblenden
        guiProductionSlotsElement.setProductionSlots(0);
    }

    // goodsSlots verzeigern
    if (productionSlots->input.isUsed()) {
        goodsSlotInput->setGoodsSlot(&productionSlots->input);
    }
    if (productionSlots->input2.isUsed()) {
        goodsSlotInput2->setGoodsSlot(&productionSlots->input2);
    }
    if (productionSlots->output.isUsed()) {
        goodsSlotOutput->setGoodsSlot(&productionSlots->output);
    }
}