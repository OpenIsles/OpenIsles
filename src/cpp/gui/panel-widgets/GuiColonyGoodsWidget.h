#ifndef _GUI_COLONY_GOODS_WIDGET_H
#define _GUI_COLONY_GOODS_WIDGET_H

#include <SDL.h>
#include "Context.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
 * @brief GUI-Panel-Widget, dass den aktuellen Lagerbestand der Siedlung anzeigt
 */
class GuiColonyGoodsWidget : public GuiPanelWidget {

public:
    GuiColonyGoodsWidget(const Context* const context);
    virtual ~GuiColonyGoodsWidget() {}

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
