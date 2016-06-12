#ifndef _GUI_SELECTED_SHIP_WIDGET_H
#define _GUI_SELECTED_SHIP_WIDGET_H

#include <SDL.h>
#include <list>
#include "global.h"
#include "gui/components/GuiGoodsSlotElement.h"
#include "gui/components/GuiPanelHeader.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
 * @brief GUI-Widget mit den Informationen des aktuell ausgewählten Schiffs.
 */
class GuiSelectedShipWidget : public GuiPanelWidget {

private:
    // Child-Elemente
    GuiPanelHeader panelHeader;                                ///< Panel-Kopfzeile

    std::list<GuiGoodsSlotElement*> goodsOnBoard;              ///< Waren an Board

public:
    GuiSelectedShipWidget(const Context& context);
    virtual ~GuiSelectedShipWidget() override;

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);

    /**
     * @brief Wird aufgerufen, wenn sich das markierte Map-Objekt ändert.
     * Das Widget passt darauf hin ggf. die Komponenten drauf an.
     *
     * @param newSelectedShip neues Schiff, was nun selektiert ist
     */
    void onSelectedShipChanged(const Ship* newSelectedShip);
};

#endif
