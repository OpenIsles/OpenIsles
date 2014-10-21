#include "game/Colony.h"

#ifndef _GUI_UTILS_H
#define _GUI_UTILS_H

class Graphic;

/**
 * @brief Klasse mit statischen Helperklassen
 */
class GuiUtils {

public:
    /**
     * @brief Zeichnet ein Kasten mit einem Warensymbol. Optional mit Warenanzeige oder Lagerbestandsbalken
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     * @param goodsType welches Gut
     * @param inventory Lagerbestand, der ausgegeben werden soll (oder -1, wenn keine Anzeige erwünscht)
     * @param capacity Lagerkapazität (-1, wenn keine Anzeige erwünscht). Wenn ungleich -1, wird statt des Bestand
     *                 rechts in der Box ein Balken angezeigt
     */
    static void drawGoodsBox(int x, int y, GoodsType goodsType, double inventory, double capacity);

};

#endif

