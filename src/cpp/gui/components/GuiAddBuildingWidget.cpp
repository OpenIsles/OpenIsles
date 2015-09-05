#include <string>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "gui/components/GuiIconWithStringElement.h"
#include "map/Street.h"
#include "utils/StringFormat.h"


GuiAddBuildingWidget::GuiAddBuildingWidget(const Context* const context) :
    GuiBase(context), productionSlotsElement(context),
    coinsElement(context), toolsElement(context), woodElement(context), bricksElement(context) {

    setCoords(0, 0, 210, 320);

    // Produktion

    productionSlotsElement.setPosition(15, 175);
    addChildElement(&productionSlotsElement);

    // Baukosten: Münzen

    const int elementsX = 7;
    const int elementsXColumnWidth = 75;
    const int elementsHeight = 30;
    const int elementsY1 = 255;
    const int elementsY2 = elementsY1 + elementsHeight + 5;

    coinsElement.setCoords(elementsX, elementsY1, 85, elementsHeight);
    coinsElement.setGraphic(context->graphicsMgr->getGraphicSet("coin")->getStatic()->getGraphic());
    coinsElement.setStatusBarText("Errichtungskosten für dieses Bauwerk");
    addChildElement(&coinsElement);

    // Baukosten: Baumaterial

    const Good* good;
    std::string graphicSetName;

    good = context->configMgr->getGood("tools");
    graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(good, false);
    toolsElement.setCoords(elementsX, elementsY2, 50, elementsHeight);
    toolsElement.setGraphic(context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic());
    toolsElement.setStatusBarText("Werkzeugbedarf für dieses Bauwerk");

    good = context->configMgr->getGood("wood");
    graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(good, false);
    woodElement.setCoords(elementsX + elementsXColumnWidth, elementsY2, 50, elementsHeight);
    woodElement.setGraphic(context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic());
    woodElement.setStatusBarText("Holzbedarf für dieses Bauwerk");

    good = context->configMgr->getGood("bricks");
    graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(good, false);
    bricksElement.setCoords(elementsX + 2*elementsXColumnWidth, elementsY2, 50, elementsHeight);
    bricksElement.setGraphic(context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic());
    bricksElement.setStatusBarText("Ziegelbedarf für dieses Bauwerk");

    addChildElement(&toolsElement);
    addChildElement(&woodElement);
    addChildElement(&bricksElement);

    // TODO Child-Buttons zum Drehen hinzufügen
}

void GuiAddBuildingWidget::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    const MapObjectType* mapObjectType = context->guiMgr->getPanelState().addingMapObject;

    // Name des Map-Objekts
    context->guiMgr->drawPanelHeader(windowX, windowY, mapObjectType->title, nullptr);

    // Grafik
    const FourthDirection& view = context->guiMgr->getPanelState().addingMapObjectView;
    const std::string graphicsSetName = context->graphicsMgr->getGraphicSetNameForMapObject(mapObjectType);
    const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicsSetName);

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

    // produzierte Waren
    const ProductionSlots& buildingProduction = mapObjectType->buildingProduction;

    if (buildingProduction.input2.isUsed()) {
        productionSlotsElement.setProductionSlots(
            ProductionSlot::INPUT | ProductionSlot::INPUT2 | ProductionSlot::OUTPUT);
        productionSlotsElement.getGoodsSlotElement(ProductionSlot::INPUT)->setGoodsSlot(&buildingProduction.input);
        productionSlotsElement.getGoodsSlotElement(ProductionSlot::INPUT2)->setGoodsSlot(&buildingProduction.input2);
        productionSlotsElement.getGoodsSlotElement(ProductionSlot::OUTPUT)->setGoodsSlot(&buildingProduction.output);
    }
    else if (buildingProduction.input.isUsed()) {
        productionSlotsElement.setProductionSlots(ProductionSlot::INPUT |  ProductionSlot::OUTPUT);
        productionSlotsElement.getGoodsSlotElement(ProductionSlot::INPUT)->setGoodsSlot(&buildingProduction.input);
        productionSlotsElement.getGoodsSlotElement(ProductionSlot::OUTPUT)->setGoodsSlot(&buildingProduction.output);
    }
    else if (buildingProduction.output.isUsed()) {
        productionSlotsElement.setProductionSlots(ProductionSlot::OUTPUT);
        productionSlotsElement.getGoodsSlotElement(ProductionSlot::OUTPUT)->setGoodsSlot(&buildingProduction.output);
    }
    else {
        productionSlotsElement.setProductionSlots(0);
    }

    // Baukosten
    coinsElement.setString(toString(mapObjectType->buildingCosts.coins));
    toolsElement.setString(toString(mapObjectType->buildingCosts.tools));
    woodElement.setString(toString(mapObjectType->buildingCosts.wood));
    bricksElement.setString(toString(mapObjectType->buildingCosts.bricks));
}
