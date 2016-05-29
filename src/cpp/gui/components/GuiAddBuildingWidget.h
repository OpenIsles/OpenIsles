#ifndef _GUI_ADD_BUILDING_WIDGET_H
#define _GUI_ADD_BUILDING_WIDGET_H

#include <SDL_render.h>
#include "global.h"
#include "gui/components/GuiBase.h"
#include "gui/components/GuiIconWithStringElement.h"
#include "gui/components/GuiPanelHeader.h"
#include "gui/components/GuiProductionSlotsElement.h"


/**
 * @brief GUI-Widget, was das Gebäude anzeigt, was aktuell platziert wird. Zusätzlich enthält es die Controls
 * zum Drehen (TODO), die Baukosten und die Infos, was es produziert.
 */
class GuiAddBuildingWidget : public GuiBase {

private:
    // GUI-Elemente
    GuiPanelHeader panelHeader;                       ///< Panel-Header mit dem Namen des neuen Gebäudes
    GuiProductionSlotsElement productionSlotsElement; ///< produzierte Waren
    GuiIconWithStringElement coinsElement;            ///< Münzkosten
    GuiIconWithStringElement operatingCostsElement;   ///< Betriebskosten
    GuiIconWithStringElement toolsElement;            ///< Werkzeugkosten
    GuiIconWithStringElement woodElement;             ///< Holzkosten
    GuiIconWithStringElement bricksElement;           ///< Ziegelkosten

public:
    GuiAddBuildingWidget(const Context& context);
    virtual ~GuiAddBuildingWidget() {}

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);

    /**
     * @brief Wird aufgerufen, wenn der Nutzer ein anderes Map-Objekt auswählt, was gebaut werden soll
     *
     * @param newAddingMapObject Map-Objekt-Typ, der grade ausgewählt wurde (`nullptr` im Abrissmodus)
     */
    void onAddingMapObjectChanged(const MapObjectType* newAddingMapObject);

};

#endif
