#include <SDL.h>
#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
//#include "gui/FontMgr.h"
#include "gui/panel-widgets/GuiColonyGoodsWidget.h"
#include "map/Map.h"


//static SDL_Color colorLightBrown = {223, 216, 183, 255};
//static SDL_Color colorBlack = {0, 0, 0, 255};


GuiColonyGoodsWidget::GuiColonyGoodsWidget(const Context* const context) : GuiPanelWidget(context) {
}

void GuiColonyGoodsWidget::renderElement(SDL_Renderer* renderer) {
    const Building* selectedBuilding = reinterpret_cast<const Building*>(context->game->getMap()->getSelectedMapObject());
    if (selectedBuilding == nullptr) {
        return;
    }

    StructureType structureType = selectedBuilding->getStructureType();
    if (structureType != OFFICE1 && structureType != MARKETPLACE) {
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
