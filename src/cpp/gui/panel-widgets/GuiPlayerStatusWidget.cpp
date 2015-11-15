#include "graphics/mgr/IFontMgr.h"
#include "gui/panel-widgets/GuiPlayerStatusWidget.h"
#include "utils/Color.h"


static Color colorWhite = Color(255, 255, 255, 255);
static Color colorBlack = Color(0, 0, 0, 255);


GuiPlayerStatusWidget::GuiPlayerStatusWidget(const Context* const context) : GuiPanelWidget(context) {
}

GuiPlayerStatusWidget::~GuiPlayerStatusWidget() {
}

void GuiPlayerStatusWidget::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    context->guiMgr->drawPanelHeader(windowX, windowY, "Spielerstatus", player);


    // Da die Texte alle keinen Statusleisten-Text haben, rendern wir direkt, statt über weitere GUI-Elemente zu gehen.
    // Geht schneller :-)
    const PlayerStatus& playerStatus = player->playerStatus;

    constexpr int flagsLeft = RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_TOP;
    constexpr int flagsRight = RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_TOP;

    const int xLeft = windowX + 5;
    const int xRight = windowX + 196;
    const int xIcon = windowX + 205;

    // Einwohner
    int y = windowY + 60;
    context->fontMgr->renderText(renderer, "Einwohner:", xLeft, y,
                                 &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsLeft);

    context->fontMgr->renderText(renderer, toString(playerStatus.population), xRight, y,
                                 &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsRight);

    context->graphicsMgr->getGraphicSet("population-man")->getStatic()->getGraphic()->drawAt(xIcon, y);

    // Steuererträge
    y += 34;
    context->fontMgr->renderText(renderer, "Steuererträge:", xLeft, y,
                                 &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsLeft);

    context->fontMgr->renderText(renderer, toString(playerStatus.taxesIncome), xRight, y,
                                 &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsRight);

    context->graphicsMgr->getGraphicSet("coin")->getStatic()->getGraphic()->drawAt(xIcon, y);
}