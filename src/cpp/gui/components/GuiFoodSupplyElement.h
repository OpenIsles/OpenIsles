#ifndef _GUI_FOOD_SUPPLY_ELEMENT_H
#define _GUI_FOOD_SUPPLY_ELEMENT_H

#include "gui/components/GuiStaticTextElement.h"
#include "Context.h"

/**
 * Statisches Element, was die Nahrungsversorgung visualisiert
 */
class GuiFoodSupplyElement : public GuiStaticTextElement {

private:
    /**
     * @brief Versorgung in Prozent (d.&nbsp;h. Werte zwischen 0 und 1)
     */
    double foodSupply = 1.0;

public:
    GuiFoodSupplyElement(const Context& context);
    virtual ~GuiFoodSupplyElement() override;

    void setFoodSupply(double foodSupply) {
        this->foodSupply = foodSupply;
    }

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) override;

};

#endif