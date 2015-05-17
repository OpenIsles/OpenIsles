#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/panel-widgets/GuiSelectedBuildingWidget.h"
#include "map/Map.h"
#include "utils/Color.h"


static Color colorBlack = Color(0, 0, 0, 255);
static Color colorWhite = Color(255, 255, 255, 255);


GuiSelectedBuildingWidget::GuiSelectedBuildingWidget(const Context* const context) : GuiPanelWidget(context) {
    // TODO Child-Buttons für Stilllegen und "Abholung verbieten"
}

void GuiSelectedBuildingWidget::renderElement(IRenderer* renderer) {
    const Building* selectedBuilding = nullptr;

    const MapObject* selectedMapObject = context->game->getMap()->getSelectedMapObject();
    if (selectedMapObject != nullptr) {
        selectedBuilding = reinterpret_cast<const Building*>(selectedMapObject);
    }

    if (selectedBuilding == nullptr) {
        // Kein Gebäude ausgewählt, dann nix zu tun
        return;
    }
    if (context->guiMgr->getPanelState().selectedPanelButton == PanelButton::ADD_BUILDING) {
        // Wenn wir ein Gebäude platzieren, dürfen wir dieses Widget nicht zeichnen, damit sich das nicht mit dem
        // GuiAddBuildingWidget überschneidet.
        return;
    }

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    const MapObjectType& mapObjectType = selectedBuilding->getMapObjectType();
    const MapObjectConfig* mapObjectConfig = context->configMgr->getMapObjectConfig(mapObjectType);

    // Gebäudename
    context->fontMgr->renderText(renderer, mapObjectConfig->getName(), windowX + width/2, windowY + 23,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 15, RENDERTEXT_HALIGN_CENTER);

    // produzierte Waren
    const ProductionSlots* productionSlots = &selectedBuilding->productionSlots;
    int productionY = windowY + 58;
    if (productionSlots->input2.isUsed()) {
        // input + input2 -> output

        context->guiMgr->drawGoodsBox(windowX + 42, productionY,
            productionSlots->input.good, productionSlots->input.inventory, -1);
        context->graphicsMgr->getGraphicSet("production-plus")->getStatic()->getGraphic()->drawAt(
            windowX + 88, productionY);
        context->guiMgr->drawGoodsBox(windowX + 103, productionY,
            productionSlots->input2.good, productionSlots->input2.inventory, -1);
        context->graphicsMgr->getGraphicSet("production-arrow")->getStatic()->getGraphic()->drawAt(
            windowX + 149, productionY);
        context->guiMgr->drawGoodsBox(windowX + 163, productionY,
            productionSlots->output.good, productionSlots->output.inventory, -1);
    }
    else if (productionSlots->input.isUsed()) {
        // input -> output

        context->guiMgr->drawGoodsBox(windowX + 73, productionY,
            productionSlots->input.good, productionSlots->input.inventory, -1);
        context->graphicsMgr->getGraphicSet("production-arrow")->getStatic()->getGraphic()->drawAt(
            windowX + 119, productionY);
        context->guiMgr->drawGoodsBox(windowX + 133, productionY,
            productionSlots->output.good, productionSlots->output.inventory, -1);
    }
    else if (productionSlots->output.isUsed()) {
        // output

        context->guiMgr->drawGoodsBox(windowX + 103, productionY,
            productionSlots->output.good, productionSlots->output.inventory, -1);
    }

    // TODO Auslastung
    // TODO Betriebskosten
}
