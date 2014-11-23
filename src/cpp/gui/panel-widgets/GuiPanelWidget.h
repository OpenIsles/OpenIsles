#ifndef _GUI_PANEL_WIDGET_H
#define _GUI_PANEL_WIDGET_H

#include <SDL_render.h>
#include "gui/components/GuiBase.h"

union SDL_Event;
class SDL_Renderer;


/**
 * @brief Basisklasse für alle GUI-Panel-Widgets.
 *
 * Das sind die verschiedenen Elemente, die im Panel rechts angezeigt werden können. Es wird jeweils immer nur
 * genau EIN Panel-Widget im Panel gezeigt. Alle Panel-Widgets befinden sich an derselben Stelle mit derselben
 * Größe.
 */
class GuiPanelWidget : public GuiBase {

public:
    GuiPanelWidget() {
        setCoords(10, 312, 236, 446);
    }
    virtual ~GuiPanelWidget() {}

};

#endif
