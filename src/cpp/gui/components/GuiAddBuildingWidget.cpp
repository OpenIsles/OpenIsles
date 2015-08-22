#include <SDL.h>
#include <string>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "map/Street.h"
#include "utils/Color.h"
#include "utils/StringFormat.h"


static Color colorBlack = Color(0, 0, 0, 255);
static Color colorWhite = Color(255, 255, 255, 255);



GuiAddBuildingWidget::GuiAddBuildingWidget(const Context* const context) :
    GuiBase(context), guiProductionSlotsElement(context) {

    setCoords(0, 0, 210, 320);

    guiProductionSlotsElement.setPosition(15, 175);
    addChildElement(&guiProductionSlotsElement);

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
        guiProductionSlotsElement.setProductionSlots(
            ProductionSlot::INPUT | ProductionSlot::INPUT2 | ProductionSlot::OUTPUT);
        guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::INPUT)->setGoodsSlot(&buildingProduction.input);
        guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::INPUT2)->setGoodsSlot(&buildingProduction.input2);
        guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::OUTPUT)->setGoodsSlot(&buildingProduction.output);
    }
    else if (buildingProduction.input.isUsed()) {
        guiProductionSlotsElement.setProductionSlots(ProductionSlot::INPUT |  ProductionSlot::OUTPUT);
        guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::INPUT)->setGoodsSlot(&buildingProduction.input);
        guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::OUTPUT)->setGoodsSlot(&buildingProduction.output);
    }
    else if (buildingProduction.output.isUsed()) {
        guiProductionSlotsElement.setProductionSlots(ProductionSlot::OUTPUT);
        guiProductionSlotsElement.getGoodsSlotElement(ProductionSlot::OUTPUT)->setGoodsSlot(&buildingProduction.output);
    }
    else {
        guiProductionSlotsElement.setProductionSlots(0);
    }

    // Baukosten
    int costsY = windowY + 235;

    context->graphicsMgr->getGraphicSet("coin")->getStatic()->getGraphic()->drawAt(windowX + 7, costsY);
    context->fontMgr->renderText(
        renderer, toString(mapObjectType->buildingCosts.coins), windowX + 42, costsY + 12,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);

    costsY += 30;

    const Good* good = context->configMgr->getGood("tools");
    std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(good, false);
    context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic()->drawAt(windowX + 7, costsY);
    context->fontMgr->renderText(
        renderer, toString(mapObjectType->buildingCosts.tools), windowX + 42, costsY + 16,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);

    good = context->configMgr->getGood("wood");
    graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(good, false);
    context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic()->drawAt(windowX + 67, costsY);
    context->fontMgr->renderText(
        renderer, toString(mapObjectType->buildingCosts.wood), windowX + 102, costsY + 16,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);

    good = context->configMgr->getGood("bricks");
    graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(good, false);
    context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic()->drawAt(windowX + 127, costsY);
    context->fontMgr->renderText(
        renderer, toString(mapObjectType->buildingCosts.bricks), windowX + 162, costsY + 16,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);
}
