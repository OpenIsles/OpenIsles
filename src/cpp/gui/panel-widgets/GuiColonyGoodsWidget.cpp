#include <list>
#include <SDL.h>
#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
//#include "gui/SDLFontMgr.h"
#include "gui/panel-widgets/GuiColonyGoodsWidget.h"
#include "map/Map.h"
//#include "utils/Color.h"


//static Color colorLightBrown = Color(223, 216, 183, 255);
//static Color colorBlack = Color(0, 0, 0, 255);


GuiColonyGoodsWidget::GuiColonyGoodsWidget(const Context* const context) : GuiPanelWidget(context) {
}

void GuiColonyGoodsWidget::renderElement(IRenderer* renderer) {
    const Building* selectedBuilding = reinterpret_cast<const Building*>(context->game->getMap()->getSelectedMapObject());
    if (selectedBuilding == nullptr) {
        return;
    }

    if (!selectedBuilding->isStorageBuilding()) {
        return; // Widget wird nur für Lagergebäude angezeigt
    }

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Name der Siedlung
    Colony* colony = context->game->getColony(selectedBuilding);

//    context->fontMgr->renderText(renderer, "Siedlungsname", windowX + width/2, windowY + 15, // TODO Siedlungsname aus Savegame laden; is da schon drin
//        &colorLightBrown, &colorBlack, "DroidSans-Bold.ttf", 15, RENDERTEXT_HALIGN_CENTER);

    // Waren
    const std::list<Good>& allGoods = context->configMgr->getAllGoodsOrdered();

    int xStart = windowX + 22;

    int x = xStart;
    int y = windowY + 68;
    int i = 0;
    for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++, i++) {
        const Good* good = &*iter;
        GoodsSlot goods = colony->getGoods(good);

        int inventory = (int) floor(goods.inventory);
        int capacity = (int) floor(goods.capacity);

        context->guiMgr->drawGoodsBox(x, y, good, inventory, capacity);

        if (i % 4 == 3) {
            x = xStart;
            y += 52;
        } else {
            x += 50;
        }
    }

}
