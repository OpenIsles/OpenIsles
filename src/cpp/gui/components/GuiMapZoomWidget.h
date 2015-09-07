#ifndef _GUI_MAP_ZOOM_WIDGET_H
#define _GUI_MAP_ZOOM_WIDGET_H

#include "Context.h"
#include "gui/components/GuiBase.h"
#include "gui/components/GuiButton.h"


/**
 * @brief GUI-Element, mit dem die Karte gezoomt werden kann
 */
class GuiMapZoomWidget : public GuiBase {

private:
    GuiButton buttonPlus;   ///< Plus-Button
    GuiButton buttonMinus;  ///< Minus-Button

public:
    /**
     * @brief Konstruktor.
     * @param context Dependency
     */
    GuiMapZoomWidget(const Context* const context);

    virtual ~GuiMapZoomWidget();

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) {
        // Nix zu tun, machen die Kinder alles
    }

};

#endif
