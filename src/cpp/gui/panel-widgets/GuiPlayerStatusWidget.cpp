#include "global.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/panel-widgets/GuiPlayerStatusWidget.h"
#include "utils/Color.h"


static Color colorWhite = Color(255, 255, 255, 255);
static Color colorBlack = Color(0, 0, 0, 255);


GuiPlayerStatusWidget::GuiPlayerStatusWidget(const Context& context) :
    GuiPanelWidget(context), panelHeader(context) {

    panelHeader.setPosition(0, 0);
    panelHeader.setText(_("Player's status"));
    addChildElement(&panelHeader);
}

GuiPlayerStatusWidget::~GuiPlayerStatusWidget() {
    childElements.clear();
}

void GuiPlayerStatusWidget::setPlayer(const Player* player) {
    this->player = player;

    panelHeader.setCoatOfArmsIcon(player);
}

void GuiPlayerStatusWidget::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Da die Texte alle keinen Statusleisten-Text haben, rendern wir direkt, statt über weitere GUI-Elemente zu gehen.
    // Geht schneller :-)
    const PlayerStatus& playerStatus = player->playerStatus;

    constexpr int flagsLeft = RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_TOP;
    constexpr int flagsRight = RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_TOP;

    const int xLeft = windowX + 5;
    const int xRight = windowX + 202;

    // Einwohner
    int y = windowY + 60;
    context.fontMgr->renderText(renderer, _("Population:"), xLeft, y,
                                &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsLeft);

    context.fontMgr->renderText(renderer, toString(playerStatus.population), xRight, y,
                                &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsRight);

    context.graphicsMgr->getGraphicSet("population-man")->getStatic()->getGraphic()->drawAt(windowX + 209, y - 5);

    // Steuererträge
    y += 34;
    context.fontMgr->renderText(renderer, _("Tax incomings:"), xLeft, y,
                                &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsLeft);

    context.fontMgr->renderText(renderer, toString(playerStatus.taxesIncome), xRight, y,
                                &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsRight);

    context.graphicsMgr->getGraphicSet("coin/coin")->getStatic()->getGraphic()->drawAt(windowX + 206, y - 5);

    // Betriebskosten
    y += 22;
    context.fontMgr->renderText(renderer, _("Operating costs:"), xLeft, y,
                                &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsLeft);

    context.fontMgr->renderText(renderer, toString(playerStatus.operatingCosts), xRight, y,
                                &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsRight);

    context.graphicsMgr->getGraphicSet("coin/coin")->getStatic()->getGraphic()->drawAt(windowX + 206, y - 5);

    // Bilanz
    y += 34;
    context.fontMgr->renderText(renderer, _("Balance:"), xLeft, y,
                                &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsLeft);

    context.fontMgr->renderText(renderer, toString(playerStatus.balance), xRight, y,
                                &colorWhite, &colorBlack, "DroidSans.ttf", 17, flagsRight);

    const std::string graphicBalance = (playerStatus.balance >= 0) ? "coin/coin-green-arrow" : "coin/coin-red-arrow";
    context.graphicsMgr->getGraphicSet(graphicBalance)->getStatic()->getGraphic()->drawAt(windowX + 206, y - 5);
}