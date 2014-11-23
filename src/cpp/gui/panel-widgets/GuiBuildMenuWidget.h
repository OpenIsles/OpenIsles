#ifndef _GUI_BUILD_MENU_WIDGET_H
#define _GUI_BUILD_MENU_WIDGET_H

#include <SDL_render.h>
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
* @brief GUI-Panel-Widget, was das Baumenü anzeigt
*/
class GuiBuildMenuWidget : public GuiPanelWidget {

public:
    GuiBuildMenuWidget();
    virtual ~GuiBuildMenuWidget() {}

    /**
    * @brief Zeichnet das Element (ohne Kinder)
    */
    virtual void renderElement(SDL_Renderer* renderer) {
        // nix tun
    }

    /**
    * @brief Callback, der ein Event handelt
    * @param event SDL-Event
    */
    virtual void onEventElement(SDL_Event& event) {
        // nix tun
    }

};

#endif