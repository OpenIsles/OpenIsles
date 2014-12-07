#ifndef _GUI_DUMMY_WIDGET_H
#define _GUI_DUMMY_WIDGET_H

#include <SDL_render.h>
#include "Context.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
* @brief GUI-Panel-Widget, was nur einen Dummy-Text anzeigt. Es wird am Ende entfernt, wenn alles fertig is.
* TODO entfern mich, wenn nicht mehr gebraucht
*/
class GuiDummyWidget : public GuiPanelWidget {

public:
    GuiDummyWidget(const Context* const context);
    virtual ~GuiDummyWidget() {}

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