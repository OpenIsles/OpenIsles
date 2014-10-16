#include <game/Colony.h>

#ifndef _GUI_UTILS_H
#define _GUI_UTILS_H

class Graphic;

/**
 * @brief Klasse mit statischen Helperklassen
 */
class GuiUtils {

public:
    /**
     * @brief Zeichnet ein Kasten mit einem Warensymbol. Optional mit Warenanzeige oder (TODO) Lagerbestandsbalken
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     * @param goodsType welches Gut
     * @param inventory Lagerbestand, der ausgegeben werden soll (oder -1, wenn keine Anzeige erwünscht)
     */
    static void drawGoodsBox(int x, int y, GoodsType goodsType, int inventory);

};

#endif

