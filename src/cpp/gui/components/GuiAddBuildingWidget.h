#ifndef _GUI_ADD_BUILDING_WIDGET_H
#define _GUI_ADD_BUILDING_WIDGET_H

#include <SDL_render.h>
#include "Context.h"
#include "gui/components/GuiBase.h"


/**
 * @brief GUI-Widget, was das Gebäude anzeigt, was aktuell platziert wird. Zusätzlich enthält es die Controls
 * zum Drehen (TODO), die Baukosten und die Infos, was es produziert.
 */
class GuiAddBuildingWidget : public GuiBase {

public:
    GuiAddBuildingWidget(const Context* const context);
    virtual ~GuiAddBuildingWidget() {}

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
