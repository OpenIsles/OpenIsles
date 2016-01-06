#include <cassert>
#include "global.h"
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/components/GuiGoodsSlotElement.h"
#include "gui/panel-widgets/GuiSelectedProductionBuildingWidget.h"
#include "map/Map.h"
#include "utils/Color.h"
#include "utils/StringFormat.h"

static Color colorWhite = Color(255, 255, 255, 255);
static Color colorBlack = Color(0, 0, 0, 255);


GuiSelectedProductionBuildingWidget::GuiSelectedProductionBuildingWidget(const Context& context) :
    GuiSelectedBuildingWidget(context), buildingName(context), guiProductionSlotsElement(context),
    operatingCostsLabel(context), operatingCosts(context), operatingCostsIcon(context) {

    buildingName.setCoords(0, 55, 236, 15);
    buildingName.setColor(&colorWhite);
    buildingName.setShadowColor(&colorBlack);
    buildingName.setFontName("DroidSans-Bold.ttf");
    buildingName.setFontSize(14);
    buildingName.setAlign(RENDERTEXT_HALIGN_CENTER | RENDERTEXT_VALIGN_TOP);
    addChildElement(&buildingName);

    guiProductionSlotsElement.setPosition(15, 80);
    addChildElement(&guiProductionSlotsElement);

    goodsSlotInput = guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::INPUT);
    goodsSlotInput2 = guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::INPUT2);
    goodsSlotOutput = guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::OUTPUT);

    goodsSlotInput->setDisplayValue(true);
    goodsSlotInput2->setDisplayValue(true);
    goodsSlotOutput->setDisplayValue(true);
    goodsSlotOutput->setStatusBarText(_("Goods in this building ready for pickup"));

    // TODO Auslastung

    // Betriebskosten
    int y = 143;

    operatingCostsLabel.setCoords(10, y, 150, 15);
    operatingCostsLabel.setText(_("Operating costs:"));
    operatingCostsLabel.setColor(&colorWhite);
    operatingCostsLabel.setShadowColor(&colorBlack);
    operatingCostsLabel.setFontName("DroidSans-Bold.ttf");
    operatingCostsLabel.setFontSize(14);
    operatingCostsLabel.setAlign(RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_TOP);
    addChildElement(&operatingCostsLabel);

    operatingCosts.setCoords(165, y, 30, 15);
    operatingCosts.setColor(&colorWhite);
    operatingCosts.setShadowColor(&colorBlack);
    operatingCosts.setFontName("DroidSans-Bold.ttf");
    operatingCosts.setFontSize(14);
    operatingCosts.setAlign(RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_TOP);
    addChildElement(&operatingCosts);

    const IGraphic* graphicIcon = context.graphicsMgr->getGraphicSet("coin/coin")->getStatic()->getGraphic();
    operatingCostsIcon.setCoords(195, y - 7, graphicIcon->getWidth(), graphicIcon->getHeight());
    operatingCostsIcon.setGraphic(graphicIcon);
    addChildElement(&operatingCostsIcon);

    // TODO Child-Buttons für Stilllegen und "Abholung verbieten"
}

void GuiSelectedProductionBuildingWidget::onSelectedMapBuildingChanged(const Building* newSelectedBuilding) {
    GuiSelectedBuildingWidget::onSelectedMapBuildingChanged(newSelectedBuilding);

    const MapObjectType* mapObjectType = newSelectedBuilding->getMapObjectType();

    buildingName.setText(_(mapObjectType->getTitleMsgid()));

    const ProductionSlots& productionSlots = newSelectedBuilding->productionSlots;
    guiProductionSlotsElement.setFromProductionSlots(productionSlots);
    
    operatingCosts.setText(toString(mapObjectType->operatingCosts.running));
    // TODO stillgelegt (Code mit EconomicsMgr refactoren, der die Finanzberechnung macht)
}

void GuiSelectedProductionBuildingWidget::renderElement(IRenderer* renderer) {
    const Building* selectedBuilding =
        reinterpret_cast<const Building*>(context.game->getMap()->getSelectedMapObject());
    assert(selectedBuilding != nullptr);

    const MapObjectType* mapObjectType = selectedBuilding->getMapObjectType();

    // produzierte Waren: Statusleisten-Texte bei Bedarf anpassen
    if (goodsSlotInput->isVisible()) {
        updateInputSlotStatusBarText(
            goodsSlotInput, mapObjectType->inputAmountForProduction,
            selectedBuilding->productionSlots.input.inventory,
            mapObjectType->buildingProduction.input.good->getTitleMsgid(),
            mapObjectType->buildingProduction.output.good->getTitleMsgid());
    }
    if (goodsSlotInput2->isVisible()) {
        updateInputSlotStatusBarText(
            goodsSlotInput2, mapObjectType->input2AmountForProduction,
            selectedBuilding->productionSlots.input2.inventory,
            mapObjectType->buildingProduction.input2.good->getTitleMsgid(),
            mapObjectType->buildingProduction.output.good->getTitleMsgid());
    }

    // TODO Auslastung
    // TODO Betriebskosten
}

void GuiSelectedProductionBuildingWidget::updateInputSlotStatusBarText(
    GuiGoodsSlotElement* goodsSlotElement, double inputAmountForProduction, unsigned int inputInventory,
    const std::string& inputLabel, const std::string& outputLabel) {

    // Ausrechnen, für wie viel Output-Güter die Input-Güter reichen
    double inputEnoughForXOutputGoods = (double) inputInventory / inputAmountForProduction;

    std::string supplyString = string_sprintf(
        _("Supply of %s (enough for another %.1ft of %s)"),
        _(inputLabel).c_str(), inputEnoughForXOutputGoods, _(outputLabel).c_str());

    goodsSlotElement->setStatusBarText(supplyString);
}