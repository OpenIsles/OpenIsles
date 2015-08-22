#ifndef _GUI_GOOD_ELEMENT_H
#define _GUI_GOOD_ELEMENT_H

#include "config/Good.h"
#include "game/GoodsSlot.h"
#include "gui/components/GuiStaticElement.h"
#include "Context.h"

/**
 * Statisches Element, was einen GoodsSlot visualisiert.
 */
class GuiGoodsSlotElement : public GuiStaticElement {

private:
    /**
     * @brief Zeiger auf den GoodsSlot, der angezeigt wird.
     * (Es ist der Regelfall, dass wir hier einen Zeiger direkt auf den Gebäude-GoodsSlot halten)
     *
     * Der GoodsSlot darf sein Gut nicht verändern, wir setzen die Grafik nur einmalig und prüfen sie nicht mehr
     * nach. Lediglich die Lagerbestände werden kontinuierlich aktualisiert.
     */
    const GoodsSlot* goodsSlot;

    /**
     * @brief `true`, um den Zahlenwert anzuzeigen
     */
    bool displayValue = false;

    /**
     * @brief `true`, um den Balken auf der rechten Seite anzuzeigen
     */
    bool displayBar = false;

public:
	GuiGoodsSlotElement(const Context* const context);
	virtual ~GuiGoodsSlotElement() override;

    void setGoodsSlot(const GoodsSlot* goodsSlot);

    void setDisplayValue(bool displayValue) {
        this->displayValue = displayValue;
    }

    void setDisplayBar(bool displayBar) {
        this->displayBar = displayBar;
    }

    /**
	 * @brief Zeichnet das Element (ohne Kinder)
	 */
	virtual void renderElement(IRenderer* renderer) override;

};

#endif
