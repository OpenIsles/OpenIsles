#ifndef _GUI_GOOD_ELEMENT_H
#define _GUI_GOOD_ELEMENT_H

#include "config/Good.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "Context.h"
#include "../../config/Good.h"

/**
 * Statisches Element, was ein Gut visualisiert.
 */
class GuiGoodElement : public GuiStaticGraphicElement {

private:
    /**
     * @brief Zeiger auf das Gut, dass angezeigt wird.
     */
    const Good* good;

    /**
     * @brief `true`, um den Zahlenwert anzuzeigen
     */
    bool displayValue = false;

    /**
     * @brief Zahlenwert
     */
    unsigned int value = 0;

    /**
     * @brief `true`, um den Balken auf der rechten Seite anzuzeigen
     */
    bool displayBar = false;

    /**
     * @brief Zustand des Balken (Wertebereich 0 bis 1).
     */
    double barValue;

public:
    GuiGoodElement(const Context* const context);
    virtual ~GuiGoodElement() override;
    
    void setGood(const Good* good);

    void setDisplayValue(bool displayValue) {
        this->displayValue = displayValue;
    }

    void setValue(unsigned int value) {
        this->value = value;
    }

    void setDisplayBar(bool displayBar) {
        this->displayBar = displayBar;
    }

    void setBarValue(double barValue) {
        this->barValue = barValue;
    }

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) override;

};

#endif
