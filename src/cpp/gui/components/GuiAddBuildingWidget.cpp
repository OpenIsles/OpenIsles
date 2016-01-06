#include <string>
#include "global.h"
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "gui/components/GuiIconWithStringElement.h"
#include "map/Street.h"
#include "utils/StringFormat.h"


GuiAddBuildingWidget::GuiAddBuildingWidget(const Context& context) :
    GuiBase(context), panelHeader(context), productionSlotsElement(context),
    coinsElement(context), operatingCostsElement(context),
    toolsElement(context), woodElement(context), bricksElement(context) {

    setCoords(0, 0, 210, 320);

    // Gebäudename
    panelHeader.setPosition(0, 0);
    addChildElement(&panelHeader);

    // Produktion

    productionSlotsElement.setPosition(15, 175);
    addChildElement(&productionSlotsElement);

    // Baukosten: Münzen und Betriebskosten

    const int elementsX = 7;
    const int elementsXColumnWidth = 75;
    const int elementsHeight = 30;
    const int elementsY1 = 255;
    const int elementsY2 = elementsY1 + elementsHeight + 5;

    coinsElement.setCoords(elementsX, elementsY1, 85, elementsHeight);
    coinsElement.setGraphic(context.graphicsMgr->getGraphicSet("coin/coin")->getStatic()->getGraphic());
    coinsElement.setStatusBarText(_("Costs for this structure"));
    addChildElement(&coinsElement);

    operatingCostsElement.setCoords(elementsX + 2*elementsXColumnWidth, elementsY1, 85, elementsHeight);
    operatingCostsElement.setGraphic(
        context.graphicsMgr->getGraphicSet("coin/coin-red-arrow")->getStatic()->getGraphic());
    operatingCostsElement.setStatusBarText(_("Operating costs for this structure"));
    addChildElement(&operatingCostsElement);

    // Baukosten: Baumaterial

    const Good* good;
    std::string graphicSetName;

    good = context.configMgr->getGood("tools");
    graphicSetName = context.graphicsMgr->getGraphicSetNameForGoodIcons(good, false);
    toolsElement.setCoords(elementsX, elementsY2, 50, elementsHeight);
    toolsElement.setGraphic(context.graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic());
    toolsElement.setStatusBarText(_("Tools requirement for this structure"));

    good = context.configMgr->getGood("wood");
    graphicSetName = context.graphicsMgr->getGraphicSetNameForGoodIcons(good, false);
    woodElement.setCoords(elementsX + elementsXColumnWidth, elementsY2, 50, elementsHeight);
    woodElement.setGraphic(context.graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic());
    woodElement.setStatusBarText(_("Wood requirement for this structure"));

    good = context.configMgr->getGood("bricks");
    graphicSetName = context.graphicsMgr->getGraphicSetNameForGoodIcons(good, false);
    bricksElement.setCoords(elementsX + 2*elementsXColumnWidth, elementsY2, 50, elementsHeight);
    bricksElement.setGraphic(context.graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic());
    bricksElement.setStatusBarText(_("Bricks requirement for this structure"));

    addChildElement(&toolsElement);
    addChildElement(&woodElement);
    addChildElement(&bricksElement);

    // TODO Child-Buttons zum Drehen hinzufügen
}

void GuiAddBuildingWidget::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    const MapObjectType* mapObjectType = context.guiMgr->getPanelState().addingMapObject;

    // Grafik
    const FourthDirection& view = context.guiMgr->getPanelState().addingMapObjectView;
    const std::string graphicsSetName = context.graphicsMgr->getGraphicSetNameForMapObject(mapObjectType);
    const GraphicSet* graphicSet = context.graphicsMgr->getGraphicSet(graphicsSetName);

    // TODO duplicate code
    const IGraphic* graphic;
    if (mapObjectType->type == MapObjectTypeClass::HARVESTABLE) {
        // Harvestable? ausgewachsenen Zustand nehmen
        unsigned char maxAge = mapObjectType->maxAge;
        const std::string fullgrownState = "growth" + toString(maxAge);
        graphic = graphicSet->getByStateAndView(fullgrownState, view)->getGraphic();
    }
    else if (mapObjectType->type == MapObjectTypeClass::STREET) {
        const std::string state = Street::STATE_NAME_EW;
        graphic = graphicSet->getByStateAndView(state, view)->getGraphic();
    }
    else {
        graphic = graphicSet->getByView(view)->getGraphic();
    }

    double scale; // ggf. verkleinert zeichnen, wenn das Gebäude zu groß is
    if (graphic-> getWidth() <= 160) {
        scale = 1;
    } else if (graphic->getWidth() <= 240) {
        scale = 0.666666666;
    } else {
        scale = 0.5;
    }

    int x = windowX + 117 - (int)(graphic->getWidth() * scale / 2);
    int y = windowY + 48;
    graphic->drawShadowScaledAt(x, y + 12, scale);
    graphic->drawScaledAt(x, y, scale);
}

void GuiAddBuildingWidget::onAddingMapObjectChanged(const MapObjectType* newAddingMapObject) {
    // Name des Map-Objekts
    panelHeader.setText(_(newAddingMapObject->getTitleMsgid()));

    // produzierte Waren
    const ProductionSlots& buildingProduction = newAddingMapObject->buildingProduction;
    productionSlotsElement.setFromProductionSlots(buildingProduction);

    // Baukosten
    const BuildingCosts& buildingCosts = newAddingMapObject->buildingCosts;
    coinsElement.setString(toString(buildingCosts.coins));
    operatingCostsElement.setString(toString(newAddingMapObject->operatingCosts.running));
    toolsElement.setString(toString(buildingCosts.tools));
    woodElement.setString(toString(buildingCosts.wood));
    bricksElement.setString(toString(buildingCosts.bricks));
}
