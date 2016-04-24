#include "global.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/panel-widgets/GuiSelectedPublicBuildingWidget.h"
#include "map/MapObjectType.h"
#include "utils/Color.h"
#include "utils/StringFormat.h"


GuiSelectedPublicBuildingWidget::GuiSelectedPublicBuildingWidget(const Context& context) :
    GuiSelectedBuildingWidget(context), buildingName(context), buildingIconElement(context),
    operatingCostsLabel(context), operatingCosts(context), operatingCostsIcon(context) {

    buildingName.setCoords(0, 55, 236, 15);
    buildingName.setColor(&Color::white);
    buildingName.setShadowColor(&Color::black);
    buildingName.setFontName("DroidSans-Bold.ttf");
    buildingName.setFontSize(14);
    buildingName.setAlign(RENDERTEXT_HALIGN_CENTER | RENDERTEXT_VALIGN_TOP);
    addChildElement(&buildingName);

    buildingIconElement.setPosition(95, 80);
    addChildElement(&buildingIconElement);

    // Betriebskosten
    int y = 143;

    operatingCostsLabel.setCoords(10, y, 150, 15);
    operatingCostsLabel.setText(_("Operating costs:"));
    operatingCostsLabel.setColor(&Color::white);
    operatingCostsLabel.setShadowColor(&Color::black);
    operatingCostsLabel.setFontName("DroidSans-Bold.ttf");
    operatingCostsLabel.setFontSize(14);
    operatingCostsLabel.setAlign(RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_TOP);
    addChildElement(&operatingCostsLabel);

    operatingCosts.setCoords(160, y, 30, 15);
    operatingCosts.setColor(&Color::white);
    operatingCosts.setShadowColor(&Color::black);
    operatingCosts.setFontName("DroidSans-Bold.ttf");
    operatingCosts.setFontSize(14);
    operatingCosts.setAlign(RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_TOP);
    addChildElement(&operatingCosts);

    const IGraphic* graphicIcon = context.graphicsMgr->getGraphicSet("coin/coin-red-arrow")->getStatic()->getGraphic();
    operatingCostsIcon.setPosition(195, y - 7);
    operatingCostsIcon.setGraphic(graphicIcon);
    addChildElement(&operatingCostsIcon);
}

void GuiSelectedPublicBuildingWidget::onSelectedMapBuildingChanged(const Building* newSelectedBuilding) {
    GuiSelectedBuildingWidget::onSelectedMapBuildingChanged(newSelectedBuilding);

    const MapObjectType* mapObjectType = newSelectedBuilding->getMapObjectType();

    buildingName.setText(_(mapObjectType->getTitleMsgid()));

    std::string graphicSetName = context.graphicsMgr->getGraphicSetNameForAddBuildingButton(mapObjectType);
    const IGraphic* graphic = context.graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic();
    buildingIconElement.setGraphic(graphic);

    operatingCosts.setText(toString(mapObjectType->operatingCosts.running));
}

void GuiSelectedPublicBuildingWidget::renderElement(IRenderer* renderer) {
}