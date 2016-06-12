#ifndef _GUI_SELECTED_HOUSE_BUILDING_WIDGET_H
#define _GUI_SELECTED_HOUSE_BUILDING_WIDGET_H

#include <SDL.h>
#include <list>
#include "global.h"
#include "gui/components/GuiFoodSupplyElement.h"
#include "gui/components/GuiGoodElement.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "gui/components/GuiStaticTextElement.h"
#include "gui/panel-widgets/GuiSelectedBuildingWidget.h"


/**
 * @brief GUI-Widget mit den Informationen des aktuell ausgewählten Hauses.
 */
class GuiSelectedHouseBuildingWidget : public GuiSelectedBuildingWidget {

private:
    // Child-Elemente
    GuiStaticGraphicElement populationSatisfaction;           ///< Zufriedenheit der Bevölkerung
    GuiStaticTextElement inhabitants;                         ///< Anzahl Einwohner
    GuiStaticTextElement populationTier;                      ///< Bevölkerungsgruppe

    std::list<GuiGoodElement*> needsGoods;                    ///< verlangte Güter
    std::list<GuiStaticGraphicElement*> needsPublicBuildings; ///< verlangte öffentliche Gebäude

    GuiFoodSupplyElement foodSupplyElement;                   ///< Nahrungsversorgung

public:
    GuiSelectedHouseBuildingWidget(const Context& context);
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
    virtual void onSelectedMapBuildingChanged(const Building* newSelectedBuilding);
};

#endif
