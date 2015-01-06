#include <SDL.h>
#include <string>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "utils/Color.h"
#include "utils/StringFormat.h"


static Color colorLightBrown = Color(223, 216, 183, 255);
static Color colorBlack = Color(0, 0, 0, 255);
static Color colorWhite = Color(255, 255, 255, 255);



GuiAddBuildingWidget::GuiAddBuildingWidget(const Context* const context) : GuiBase(context) {
    setCoords(13, 8, 210, 320);

    // TODO Child-Buttons zum Drehen hinzufügen
}

void GuiAddBuildingWidget::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    StructureType structureType = context->guiMgr->getPanelState().addingStructure;
    const BuildingConfig* buildingConfig = context->configMgr->getBuildingConfig(structureType);

    // Gebäudename
    context->fontMgr->renderText(renderer, buildingConfig->getName(), windowX + width/2, windowY + 15,
        &colorLightBrown, &colorBlack, "DroidSans-Bold.ttf", 15, RENDERTEXT_HALIGN_CENTER);

    // Gebäude-Grafik
    const std::string buildingGraphicsSetName = context->graphicsMgr->getGraphicSetNameForStructure(structureType);
    const GraphicSet* buildingGraphicsSet = context->graphicsMgr->getGraphicSet(buildingGraphicsSetName);
    const IGraphic* buildingGraphic = buildingGraphicsSet->getByView("south")->getGraphic(); // TODO richtige View nehmen

    double scale; // ggf. verkleinert zeichnen, wenn das Gebäude zu groß is
    if (buildingGraphic-> getWidth() <= 160) {
        scale = 1;
    } else if (buildingGraphic->getWidth() <= 240) {
        scale = 0.666666666;
    } else {
        scale = 0.5;
    }

    int x = windowX + 110 - (int)(buildingGraphic->getWidth() * scale / 2);
    int y = windowY + 40;
    buildingGraphic->drawScaledAt(x, y, scale);

    // produzierte Waren
    const ProductionSlots* buildingProduction = buildingConfig->getBuildingProduction();
    int productionY = windowY + 160;
    if (buildingProduction->input2.isUsed()) {
        // input + input2 -> output

        context->guiMgr->drawGoodsBox(windowX + 29, productionY, buildingProduction->input.goodsType, -1, -1);
        context->graphicsMgr->getGraphicSet("production-plus")->getStatic()->getGraphic()->drawAt(windowX + 75, productionY);
        context->guiMgr->drawGoodsBox(windowX + 90, productionY, buildingProduction->input2.goodsType, -1, -1);
        context->graphicsMgr->getGraphicSet("production-arrow")->getStatic()->getGraphic()->drawAt(windowX + 136, productionY);
        context->guiMgr->drawGoodsBox(windowX + 150, productionY, buildingProduction->output.goodsType, -1, -1);
    }
    else if (buildingProduction->input.isUsed()) {
        // input -> output

        context->guiMgr->drawGoodsBox(windowX + 60, productionY, buildingProduction->input.goodsType, -1, -1);
        context->graphicsMgr->getGraphicSet("production-arrow")->getStatic()->getGraphic()->drawAt(windowX + 106, productionY);
        context->guiMgr->drawGoodsBox(windowX + 120, productionY, buildingProduction->output.goodsType, -1, -1);
    }
    else if (buildingProduction->output.isUsed()) {
        // output

        context->guiMgr->drawGoodsBox(windowX + 90, productionY, buildingProduction->output.goodsType, -1, -1);
    }

    // Baukosten
    int costsY = productionY + 60;

    context->graphicsMgr->getGraphicSet("coin")->getStatic()->getGraphic()->drawAt(windowX, costsY);
    context->fontMgr->renderText(renderer, toString(buildingConfig->getBuildingCosts()->coins), windowX + 35, costsY + 12,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);

    costsY += 30;

    std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(GoodsType::TOOLS, false);
    context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic()->drawAt(windowX, costsY);
    context->fontMgr->renderText(renderer, toString(buildingConfig->getBuildingCosts()->tools), windowX + 35, costsY + 16,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);

    graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(GoodsType::WOOD, false);
    context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic()->drawAt(windowX + 60, costsY);
    context->fontMgr->renderText(renderer, toString(buildingConfig->getBuildingCosts()->wood), windowX + 95, costsY + 16,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);

    graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(GoodsType::BRICKS, false);
    context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic()->drawAt(windowX + 120, costsY);
    context->fontMgr->renderText(renderer, toString(buildingConfig->getBuildingCosts()->bricks), windowX + 155, costsY + 16,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);

}
