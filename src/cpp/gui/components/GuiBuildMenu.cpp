#include <cmath>
#include "graphics/mgr/IGraphicsMgr.h"
#include "gui/components/GuiBuildMenu.h"
#include "gui/GuiMgr.h"


GuiBuildMenu::GuiBuildMenu(const Context& context) : GuiBase(context) {
    graphicPanelTop = context.graphicsMgr->getGraphicSet("add-building/panel-top")->getStatic()->getGraphic();
    graphicPanel = context.graphicsMgr->getGraphicSet("add-building/panel")->getStatic()->getGraphic();
    graphicPanelBottom = context.graphicsMgr->getGraphicSet("add-building/panel-bottom")->getStatic()->getGraphic();
}

GuiBuildMenu::~GuiBuildMenu() {
}

void GuiBuildMenu::setButtons(std::list<const MapObjectType*>& newButtons) {
    int rows = int(std::ceil(double(newButtons.size()) / 4));
    int newHeight = graphicPanelTop->getHeight() + rows * graphicPanel->getHeight() + graphicPanelBottom->getHeight();

    this->rows = rows;
    setCoords(x, y, graphicPanel->getWidth(), newHeight);

    // Buttons setzen
    // TODO untersuchen, ob es nicht performanter ist, bestehende Buttons wiederzuverwenden
    for (GuiBase* childElement : buttons) {
        removeChildElement(childElement);
        delete childElement;
    }
    buttons.clear();

    int i = 0;
    for (const MapObjectType* mapObjectType : newButtons) {
        std::string graphicSetName;
        if (mapObjectType != nullptr) {
            graphicSetName = context.graphicsMgr->getGraphicSetNameForAddBuildingButton(mapObjectType);
        } else {
            graphicSetName = "add-building-button/question-mark";
        }

        const IGraphic* graphicAddBuildingButton =
            context.graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic();

        GuiButton* guiButton = new GuiButton(context);
        guiButton->setPosition(12 + 58 * (i % 4), 13 + 58 * (i / 4));
        guiButton->setGraphic(graphicAddBuildingButton);
        guiButton->setGraphicPressed(graphicAddBuildingButton);

        if (mapObjectType != nullptr) {
            guiButton->setStatusBarText(_(mapObjectType->getTitleMsgid()));
            guiButton->setOnClickFunction([ this, mapObjectType ]() {
                context.guiMgr->setMapObjectToBuild(mapObjectType);
            });
        }

        buttons.push_back(guiButton);
        addChildElement(guiButton);

        i++;
    }
}

void GuiBuildMenu::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Oberer Teil
    graphicPanelTop->drawAt(windowX, windowY);
    windowY += graphicPanelTop->getHeight();

    // Mittelteil
    for (int i = 0; i < rows; i++) {
        graphicPanel->drawAt(windowX, windowY);
        windowY += graphicPanel->getHeight();
    }

    // Unterer Teil
    graphicPanelBottom->drawAt(windowX, windowY);
}