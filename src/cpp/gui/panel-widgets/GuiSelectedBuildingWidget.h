#ifndef _GUI_SELECTED_BUILDING_WIDGET_H
#define _GUI_SELECTED_BUILDING_WIDGET_H

#include <SDL.h>
#include "Context.h"
#include "gui/components/GuiPanelHeader.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
 * @brief abstrakte Basisklasse für alle GUI-Widgets, die Informationen zum aktuell ausgewählten Gebäude anzeigen.
 */
class GuiSelectedBuildingWidget : public GuiPanelWidget {

private:
    GuiPanelHeader panelHeader;

public:
    GuiSelectedBuildingWidget(const Context& context);
    virtual ~GuiSelectedBuildingWidget() override;

    /**
     * @brief Wird aufgerufen, wenn sich das markierte Map-Objekt ändert.
     * Das Widget passt darauf hin ggf. die Komponenten drauf an.
     *
     * Unterklassen überschreiben und erweitern diese Methode.
     *
     * @param newSelectedBuilding neues Gebäude, was nun selektiert ist
     */
    virtual void onSelectedMapBuildingChanged(const Building* newSelectedBuilding);

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) = 0;
};

#endif
