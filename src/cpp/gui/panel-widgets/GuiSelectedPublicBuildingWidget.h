#ifndef _GUI_SELECTED_PUBLIC_BUILDING_WIDGET_H
#define _GUI_SELECTED_PUBLIC_BUILDING_WIDGET_H

#include <SDL.h>
#include <string>
#include "global.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "gui/components/GuiStaticTextElement.h"
#include "gui/panel-widgets/GuiSelectedBuildingWidget.h"


/**
 * @brief GUI-Widget mit den Informationen des aktuell ausgewählten öffentlichen Gebäudes.
 */
class GuiSelectedPublicBuildingWidget : public GuiSelectedBuildingWidget {

private:
    GuiStaticTextElement buildingName;           ///< Gebäudename
    GuiStaticGraphicElement buildingIconElement; ///< Symbol des Gebäudes

    GuiStaticTextElement operatingCostsLabel;    ///< Betriebskosten-Label
    GuiStaticTextElement operatingCosts;         ///< variables Textfeld mit die Betriebskosten
    GuiStaticGraphicElement operatingCostsIcon;  ///< Symbol für Betriebskosten

public:
    GuiSelectedPublicBuildingWidget(const Context& context);
    virtual ~GuiSelectedPublicBuildingWidget() override {}

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);

    /**
     * @brief Wird aufgerufen, wenn sich das markierte Map-Objekt ändert.
     * Das Widget passt darauf hin ggf. die Komponenten drauf an.
     *
     * @param newSelectedBuilding neues Gebäude, was nun selektiert ist
     */
    virtual void onSelectedMapBuildingChanged(const Building* newSelectedBuilding);
};

#endif
