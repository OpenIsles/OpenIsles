#ifndef _GUI_GOODS_SLOT_ELEMENT_H
#define _GUI_GOODS_SLOT_ELEMENT_H

#include "config/Good.h"
#include "game/GoodsSlot.h"
#include "gui/components/GuiGoodElement.h"
#include "Context.h"

/**
 * Statisches Element, was einen GoodsSlot visualisiert.
 */
class GuiGoodsSlotElement : public GuiGoodElement {

private:
    /**
     * @brief Zeiger auf den GoodsSlot, der angezeigt wird.
     * (Es ist der Regelfall, dass wir hier einen Zeiger direkt auf den Gebäude-GoodsSlot halten)
     *
     * Der GoodsSlot darf sein Gut nicht verändern, wir setzen die Grafik nur einmalig und prüfen sie nicht mehr
     * nach. Lediglich die Lagerbestände werden kontinuierlich aktualisiert.
     */
    const GoodsSlot* goodsSlot;

public:
    GuiGoodsSlotElement(const Context& context);
    virtual ~GuiGoodsSlotElement() override;

    void setGoodsSlot(const GoodsSlot* goodsSlot);

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) override;

};

#endif
