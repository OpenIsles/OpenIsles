#ifndef _GUI_RESOURCES_BAR_H
#define _GUI_RESOURCES_BAR_H

#include <SDL_events.h>
#include "Context.h"
#include "config/ConfigMgr.h"
#include "graphics/renderer/IRenderer.h"
#include "gui/components/GuiBase.h"

#ifdef DEBUG
#include "graphics/graphic/sdl/SDLGraphic.h"
#endif


/**
 * @brief GUI-Element, was die Resource-Leiste darstellt.
 */
class GuiResourcesBar : public GuiBase {

private:
    /**
     * @brief `true`, um die Baukosten (buildingCosts) anzuzeigen.
     */
    bool drawBuildingCosts = false;

    /**
     * @brief Baukosten
     */
    BuildingCosts buildingCosts;

public:
    /**
     * @brief Konstruktor.
     * @param context Dependency
     */
    GuiResourcesBar(const Context* const context);

    virtual ~GuiResourcesBar();

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);

    /**
     * @brief Versteckt die Baukostenanzeige
     */
    void hideBuildingCosts() {
        drawBuildingCosts = false;
    }

    /**
     * @brief Zeigt die Baukostenanzeige an bzw. ändert die Baukosten
     * @param buildingCosts Baukosten, die angezeigt werden sollen
     */
    void showBuildingCosts(const BuildingCosts& buildingCosts) {
        drawBuildingCosts = true;
        this->buildingCosts = buildingCosts;
    }
};

#endif
