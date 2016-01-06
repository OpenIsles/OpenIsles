#include "global.h"
#include "game/Game.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "gui/components/GuiMapZoomWidget.h"


GuiMapZoomWidget::GuiMapZoomWidget(const Context& context) :
    GuiBase(context), buttonPlus(context), buttonMinus(context) {

    setCoords(138, 190, 90, 90);

    const GraphicSet* graphicSet = context.graphicsMgr->getGraphicSet("map-zoom");

    buttonPlus.setCoords(0, 0, 45, 90);
    buttonPlus.setStatusBarText(_("Zoom in"));
    buttonPlus.setGraphic(graphicSet->getByState("plus")->getGraphic());
    buttonPlus.setGraphicPressed(graphicSet->getByState("plus_pressed")->getGraphic());
    buttonPlus.setOnClickFunction([this, context]() {
        context.guiMgr->increaseMapZoom();
    });

    buttonMinus.setCoords(45, 0, 45, 90);
    buttonMinus.setStatusBarText(_("Zoom out"));
    buttonMinus.setGraphic(graphicSet->getByState("minus")->getGraphic());
    buttonMinus.setGraphicPressed(graphicSet->getByState("minus_pressed")->getGraphic());
    buttonMinus.setOnClickFunction([this, context]() {
        context.guiMgr->decreaseMapZoom();
    });

    addChildElement(&buttonPlus);
    addChildElement(&buttonMinus);
}

GuiMapZoomWidget::~GuiMapZoomWidget() {
}