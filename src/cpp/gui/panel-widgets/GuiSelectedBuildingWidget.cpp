#include "gui/panel-widgets/GuiSelectedBuildingWidget.h"


GuiSelectedBuildingWidget::GuiSelectedBuildingWidget(const Context* const context) :
    GuiPanelWidget(context), panelHeader(context) {

    panelHeader.setPosition(0, 0);
    addChildElement(&panelHeader);
}

GuiSelectedBuildingWidget::~GuiSelectedBuildingWidget() {
    childElements.clear();
}

void GuiSelectedBuildingWidget::onSelectedMapBuildingChanged(const Building* newSelectedBuilding) {
    assert(newSelectedBuilding != nullptr);

    panelHeader.setText(newSelectedBuilding->getColony()->name);
}
