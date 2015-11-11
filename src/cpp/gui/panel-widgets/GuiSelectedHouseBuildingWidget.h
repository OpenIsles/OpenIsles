#ifndef _GUI_SELECTED_HOUSE_BUILDING_WIDGET_H
#define _GUI_SELECTED_HOUSE_BUILDING_WIDGET_H

#include <SDL.h>
#include <string>
#include "Context.h"
#include "graphics/graphic/sdl/SDLGraphic.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "gui/components/GuiStaticTextElement.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
 * @brief GUI-Widget mit den Informationen des aktuell ausgewählten Hauses.
 */
class GuiSelectedHouseBuildingWidget : public GuiPanelWidget {

private:
    // Child-Elemente
    GuiStaticGraphicElement populationSatisfaction; ///< Zufriedenheit der Bevölkerung
    GuiStaticTextElement inhabitants;               ///< Anzahl Einwohner
    GuiStaticTextElement populationTier;            ///< Bevölkerungsgruppe

    /**
     * @brief Grafiken der Zufriedenheit
     */
    IGraphic* populationSatisfactionGraphics[4];

public:
    GuiSelectedHouseBuildingWidget(const Context* const context);
    virtual ~GuiSelectedHouseBuildingWidget() override;

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
    void onSelectedMapBuildingChanged(const Building* newSelectedBuilding);
};

#endif
