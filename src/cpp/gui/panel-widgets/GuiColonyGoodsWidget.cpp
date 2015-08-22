#include <list>
#include <SDL.h>
#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "gui/panel-widgets/GuiColonyGoodsWidget.h"
#include "map/Map.h"
#include "utils/StringFormat.h"


GuiColonyGoodsWidget::GuiColonyGoodsWidget(const Context* const context) : GuiPanelWidget(context) {
    // GUI-Elemente für die Güter anlegen
    const int xStart = 22;

    int x = xStart;
    int y = 68;
    int i = 0;

    const std::list<const Good*>& allGoods = context->configMgr->getAllGoodsOrdered();
    for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++, i++) {
        const Good* good = *iter;

        GuiGoodsSlotElement* guiGoodsSlotElement = new GuiGoodsSlotElement(context);
        guiGoodsSlotElement->setPosition(x, y);
        guiGoodsSlotElement->setDisplayValue(false);

        guiGoodsSlotElements[good] = guiGoodsSlotElement;

        addChildElement(guiGoodsSlotElement);

        if (i % 4 == 3) {
            x = xStart;
            y += 52;
        } else {
            x += 50;
        }
    }
}

GuiColonyGoodsWidget::~GuiColonyGoodsWidget() {
    for (auto iter = guiGoodsSlotElements.cbegin(); iter != guiGoodsSlotElements.cend(); iter++) {
        GuiGoodsSlotElement* guiGoodsSlotElement = iter->second;
        delete guiGoodsSlotElement;
    }
    guiGoodsSlotElements.clear();
}

void GuiColonyGoodsWidget::onSelectedMapBuildingChanged(const Building* newSelectedBuilding) {
    assert(newSelectedBuilding != nullptr);

    colony = context->game->getColony(newSelectedBuilding);
    assert(colony != nullptr);

    for (auto iter = guiGoodsSlotElements.cbegin(); iter != guiGoodsSlotElements.cend(); iter++) {
        const Good* good = iter->first;
        GuiGoodsSlotElement* guiGoodsSlotElement = iter->second;

        guiGoodsSlotElement->setGoodsSlot(&colony->getGoods(good));
    }
}

void GuiColonyGoodsWidget::renderElement(IRenderer* renderer) {
    // Name der Siedlung
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    const Building* selectedBuilding = reinterpret_cast<const Building*>(context->game->getMap()->getSelectedMapObject());
    Colony* colony = context->game->getColony(selectedBuilding);
    context->guiMgr->drawPanelHeader(windowX, windowY, colony->name, nullptr);

    // Waren
    for (auto iter = guiGoodsSlotElements.cbegin(); iter != guiGoodsSlotElements.cend(); iter++) {
        const Good* good = iter->first;
        GuiGoodsSlotElement* guiGoodsSlotElement = iter->second;

        const GoodsSlot& goodsSlot = colony->getGoods(good);
        guiGoodsSlotElement->setStatusBarText(toString((int) goodsSlot.inventory) + "t " + good->label);
    }
}