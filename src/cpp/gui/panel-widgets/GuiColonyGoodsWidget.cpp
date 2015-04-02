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

    const MapObjectType& mapObjectType = selectedBuilding->getMapObjectType();
    if (mapObjectType != OFFICE1 && mapObjectType != OFFICE2 && mapObjectType != MARKETPLACE) {
        return; // Widget wird nur für Kontor und Marktplatz angezeigt
    }

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Name der Siedlung
    Colony* colony = context->game->getColony(selectedBuilding);

//    context->fontMgr->renderText(renderer, "Siedlungsname", windowX + width/2, windowY + 15, // TODO Siedlungsname aus Savegame laden; is da schon drin
//        &colorLightBrown, &colorBlack, "DroidSans-Bold.ttf", 15, RENDERTEXT_HALIGN_CENTER);

    // Waren
    int xStart = windowX + 22;

    int x = xStart;
    int y = windowY + 68;
    for (int i = 0; i < GoodsType::MAX_GOOD; i++) {
        GoodsType goodsType = (GoodsType) i;
        GoodsSlot goods = colony->getGoods(goodsType);

        int inventory = (int) floor(goods.inventory);
        int capacity = (int) floor(goods.capacity);

        context->guiMgr->drawGoodsBox(x, y, goodsType, inventory, capacity);

        if (i % 4 == 3) {
            x = xStart;
            y += 52;
        } else {
            x += 50;
        }
    }

}
