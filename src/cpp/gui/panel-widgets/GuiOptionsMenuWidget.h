#ifndef _GUI_OPTIONS_MENU_WIDGET_H
#define _GUI_OPTIONS_MENU_WIDGET_H

#include <SDL_render.h>
#include "Context.h"
#include "gui/components/GuiPushButton.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
 * @brief GUI-Panel-Widget, was das Optionen-Menü anzeigt
 */
class GuiOptionsMenuWidget : public GuiPanelWidget {

public:
    GuiOptionsMenuWidget(const Context* const context);
    virtual ~GuiOptionsMenuWidget() {}

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);

};

#endif