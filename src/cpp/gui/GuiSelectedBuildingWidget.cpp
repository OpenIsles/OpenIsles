#include <map/Map.h>
#include "config/BuildingConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "gui/FontMgr.h"
#include "gui/GuiSelectedBuildingWidget.h"
#include "gui/GuiUtils.h"


static SDL_Color colorBlack = {0, 0, 0, 255};
static SDL_Color colorWhite = {255, 255, 255, 255};

// Aus main.cpp importiert
extern BuildingConfigMgr* buildingConfigMgr;
extern FontMgr* fontMgr;
extern Game* game;
extern GraphicsMgr* graphicsMgr;
extern Map* map;


GuiSelectedBuildingWidget::GuiSelectedBuildingWidget() {
    setCoords(8, 0, 210, 320);

    // TODO Child-Buttons für Stilllegen und "Abholung verbieten"
}

void GuiSelectedBuildingWidget::renderElement(SDL_Renderer* renderer) {
    const Building* selectedBuilding = nullptr;

    const MapObject* selectedMapObject = map->getSelectedMapObject();
    if (selectedMapObject != nullptr) {
        selectedBuilding = reinterpret_cast<const Building*>(selectedMapObject);
    }

    if (selectedBuilding == nullptr) {
        // Kein Gebäude ausgewählt, dann nix zu tun
        return;
    }
    if (game->isAddingStructure()) {
        // Wenn wir ein Gebäude platzieren, dürfen wir dieses Widget nicht zeichnen, damit sich das nicht mit dem
        // GuiAddBuildingWidget überschneidet.
        return;
    }

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    StructureType structureType = selectedBuilding->getStructureType();
    const BuildingConfig* buildingConfig = buildingConfigMgr->getConfig(structureType);

    // Gebäudename
    fontMgr->renderText(renderer, buildingConfig->getName(), windowX + width/2, windowY + 15,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 15, RENDERTEXT_HALIGN_CENTER);

    // produzierte Waren
    const BuildingProduction* buildingProduction = buildingConfig->getBuildingProduction();
    int productionY = windowY + 50;
    if (buildingProduction->input2.isUsed()) {
        // input + input2 -> output

        GuiUtils::drawGoodsBox(windowX + 29, productionY, buildingProduction->input.goodsType, 42);
        graphicsMgr->getOtherGraphic(OtherGraphic::PRODUCTION_PLUS)->drawAt(windowX + 75, productionY);
        GuiUtils::drawGoodsBox(windowX + 90, productionY, buildingProduction->input2.goodsType, 42);
        graphicsMgr->getOtherGraphic(OtherGraphic::PRODUCTION_ARROW)->drawAt(windowX + 136, productionY);
        GuiUtils::drawGoodsBox(windowX + 150, productionY, buildingProduction->output.goodsType, 42);
    }
    else if (buildingProduction->input.isUsed()) {
        // input -> output

        GuiUtils::drawGoodsBox(windowX + 60, productionY, buildingProduction->input.goodsType, 42);
        graphicsMgr->getOtherGraphic(OtherGraphic::PRODUCTION_ARROW)->drawAt(windowX + 106, productionY);
        GuiUtils::drawGoodsBox(windowX + 120, productionY, buildingProduction->output.goodsType, 42);
    }
    else if (buildingProduction->output.isUsed()) {
        // output

        GuiUtils::drawGoodsBox(windowX + 90, productionY, buildingProduction->output.goodsType, 42);
    }

    // TODO Auslastung
    // TODO Betriebskosten
}
