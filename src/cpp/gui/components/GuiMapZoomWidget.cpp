#include "global.h"
#include "game/Game.h"
#include "graphics/graphic/GraphicSetKeyState.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "gui/components/GuiMapZoomWidget.h"


GuiMapZoomWidget::GuiMapZoomWidget(const Context& context) :
    GuiBase(context), buttonPlus(context), buttonMinus(context) {

    setCoords(138, 190, 90, 90);

    const GraphicSet* graphicSet = context.graphicsMgr->getGraphicSet("map-zoom");

    buttonPlus.setCoords(0, 0, 45, 90);
    buttonPlus.setStatusBarText(_("Zoom in"));
    buttonPlus.setGraphic(graphicSet->getByState(GraphicSetKeyState::PLUS)->getGraphic());
    buttonPlus.setGraphicPressed(graphicSet->getByState(GraphicSetKeyState::PLUS_PRESSED)->getGraphic());
    buttonPlus.setOnClickFunction([&context]() {
        context.guiMgr->increaseMapZoom();
    });

    buttonMinus.setCoords(45, 0, 45, 90);
    buttonMinus.setStatusBarText(_("Zoom out"));
    buttonMinus.setGraphic(graphicSet->getByState(GraphicSetKeyState::MINUS)->getGraphic());
    buttonMinus.setGraphicPressed(graphicSet->getByState(GraphicSetKeyState::MINUS_PRESSED)->getGraphic());
    buttonMinus.setOnClickFunction([&context]() {
        context.guiMgr->decreaseMapZoom();
    });

    addChildElement(&buttonPlus);
    addChildElement(&buttonMinus);
}

GuiMapZoomWidget::~GuiMapZoomWidget() {
}