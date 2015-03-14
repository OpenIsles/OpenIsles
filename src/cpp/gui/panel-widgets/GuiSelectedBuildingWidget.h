#ifndef _GUI_SELECTED_BUILDING_WIDGET_H
#define _GUI_SELECTED_BUILDING_WIDGET_H

#include <SDL.h>
#include "Context.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
 * @brief GUI-Widget mit den Informationen des aktuell ausgewählten Gebäudes.
 */
class GuiSelectedBuildingWidget : public GuiPanelWidget {

public:
    GuiSelectedBuildingWidget(const Context* const context);
    virtual ~GuiSelectedBuildingWidget() {}

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);

    /**
     * @brief Callback, der ein Event handelt
     * @param event SDL-Event
     * @return `true` um das Event an weitere GUI-Elemente zu reichen,
     *         `false` um anzudeuten, dass das Event bereits verarbeitet wurde. Es wird dann nicht weiter zugestellt.
     */
    virtual bool onEventElement(SDL_Event& event) {
        // nix tun
        return true;
    }

};

#endif
