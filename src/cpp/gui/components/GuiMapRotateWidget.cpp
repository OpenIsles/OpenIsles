#include "global.h"
#include "game/Game.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "gui/components/GuiMapRotateWidget.h"
#include "map/Map.h"


GuiMapRotateWidget::GuiMapRotateWidget(const Context& context) :
    GuiBase(context), buttonLeft(context), buttonRight(context) {

    setCoords(28, 190, 90, 90);

    buttonLeft.setCoords(0, 0, 45, 90);
    buttonLeft.setStatusBarText(_("Rotate map counterclockwise"));
    buttonLeft.setOnClickFunction([this, context]() {
        context.game->getMap()->rotateViewCounterclockwise();
        updateButtonGraphics();
    });

    buttonRight.setCoords(45, 0, 45, 90);
    buttonRight.setStatusBarText(_("Rotate map clockwise"));
    buttonRight.setOnClickFunction([this, context]() {
        context.game->getMap()->rotateViewClockwise();
        updateButtonGraphics();
    });

    addChildElement(&buttonLeft);
    addChildElement(&buttonRight);
}

GuiMapRotateWidget::~GuiMapRotateWidget() {
}

void GuiMapRotateWidget::updateButtonGraphics() {
    const FourthDirection& view = context.game->getMap()->getScreenView();
    const GraphicSet* graphicSet = context.graphicsMgr->getGraphicSet("map-rotate");

    buttonLeft.setGraphic(graphicSet->getByStateAndView("left", view)->getGraphic());
    buttonLeft.setGraphicPressed(graphicSet->getByStateAndView("left_pressed", view)->getGraphic());

    buttonRight.setGraphic(graphicSet->getByStateAndView("right", view)->getGraphic());
    buttonRight.setGraphicPressed(graphicSet->getByStateAndView("right_pressed", view)->getGraphic());
}

void GuiMapRotateWidget::onMapRotated() {
    updateButtonGraphics();
}
