#ifndef _GUI_BUILD_MENU_WIDGET_H
#define _GUI_BUILD_MENU_WIDGET_H

#include <SDL_render.h>
#include "Context.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
 * @brief GUI-Panel-Widget, was das Baumenü anzeigt
 */
class GuiBuildMenuWidget : public GuiPanelWidget {

public:
    GuiBuildMenuWidget(const Context* const context);
    virtual ~GuiBuildMenuWidget() {}

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) {
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