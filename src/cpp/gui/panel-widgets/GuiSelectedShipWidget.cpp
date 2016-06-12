#include <cassert>
#include "global.h"
#include "game/Game.h"
#include "gui/panel-widgets/GuiSelectedShipWidget.h"


GuiSelectedShipWidget::GuiSelectedShipWidget(const Context& context) : GuiPanelWidget(context), panelHeader(context) {
    addChildElement(&panelHeader);
}

GuiSelectedShipWidget::~GuiSelectedShipWidget() {
}

void GuiSelectedShipWidget::renderElement(IRenderer* renderer) {
    // Nix zu tun, machen die Kinder alles
}

void GuiSelectedShipWidget::onSelectedShipChanged(const Ship* newSelectedShip) {
    assert(newSelectedShip->getMapObjectType()->type == MapObjectTypeClass::SHIP);
    
    // Name
    panelHeader.setText(newSelectedShip->getName());

    // Waren
    // TODO untersuchen, ob es nicht performanter ist, immer 4 (später 8)(? aus der Config ausrechnen) Elemente anzulegen und ein-/auszublenden bzw. abzuändern
    for (GuiGoodsSlotElement* goodsSlotElement : goodsOnBoard) {
        removeChildElement(goodsSlotElement);
        delete goodsSlotElement;
    }
    goodsOnBoard.clear();

    int posIndex = 0;
    for (const GoodsSlot& goodsSlot : newSelectedShip->goodsOnBoard) {
        std::string statusBarText = (goodsSlot.isUsed()) ?
                toString(goodsSlot.inventory) + "t " + _(goodsSlot.good->getTitleMsgid()) : _("Empty");

        GuiGoodsSlotElement* goodsSlotElement = new GuiGoodsSlotElement(context);
        goodsSlotElement->setPosition(10 + (posIndex % 4) * 58, 300 + (posIndex / 4) * 58);
        goodsSlotElement->setStatusBarText(statusBarText);
        goodsSlotElement->setGoodsSlot(&goodsSlot);
        goodsSlotElement->setDisplayBar(true);
        goodsSlotElement->setDisplayValue(false);

        goodsOnBoard.push_back(goodsSlotElement);
        addChildElement(goodsSlotElement);

        posIndex++;
    }
}