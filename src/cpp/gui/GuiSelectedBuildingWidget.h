#ifndef _GUI_SELECTED_BUILDING_WIDGET_H
#define _GUI_SELECTED_BUILDING_WIDGET_H

#include <SDL.h>
#include "gui/GuiBase.h"

union SDL_Event;
class SDL_Renderer;


/**
 * @brief GUI-Widget mit den Informationen des aktuell ausgewählten Gebäudes.
 */
class GuiSelectedBuildingWidget : public GuiBase {

public:
    GuiSelectedBuildingWidget();
    virtual ~GuiSelectedBuildingWidget() {}

    /**
    * @brief Zeichnet das Element (ohne Kinder)
    */
    virtual void renderElement(SDL_Renderer* renderer);

    /**
    * @brief Callback, der ein Event handelt
    * @param event SDL-Event
    */
    virtual void onEventElement(SDL_Event& event) {
        // nix tun
    }

};

#endif
