#include "gui/FontMgr.h"
#include "gui/GuiUtils.h"

static SDL_Color colorBlack = {0, 0, 0, 255};
static SDL_Color colorWhite = {255, 255, 255, 255};

// Aus main.cpp importiert
extern FontMgr* fontMgr;
extern GraphicsMgr* graphicsMgr;
extern SDL_Renderer* renderer;


void GuiUtils::drawGoodsBox(int x, int y, GoodsType goodsType, double inventory) {
    graphicsMgr->getGraphicForGoodsMarketplaceIcon(goodsType)->drawAt(x, y);

    if (inventory != -1) {
        char inventoryOutput[10];
        sprintf(inventoryOutput, "%.0ft", floor(inventory));
        fontMgr->renderText(renderer, inventoryOutput, x + 40, y + 42, &colorWhite, &colorBlack,
            "DroidSans.ttf", 12, RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_BOTTOM);
    }
}