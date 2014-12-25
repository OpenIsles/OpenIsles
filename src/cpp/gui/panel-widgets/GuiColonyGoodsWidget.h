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
     */
    virtual void onEventElement(SDL_Event& event) {
        // nix tun
    }

};

#endif
