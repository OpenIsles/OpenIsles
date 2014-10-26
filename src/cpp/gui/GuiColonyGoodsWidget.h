#ifndef _GUI_COLONY_GOODS_WIDGET_H
#define _GUI_COLONY_GOODS_WIDGET_H

#include <SDL.h>
#include "gui/GuiBase.h"

union SDL_Event;
class SDL_Renderer;


/**
* @brief GUI-Widget, dass den aktuellen Lagerbestand der Siedlung anzeigt
*/
class GuiColonyGoodsWidget : public GuiBase {

public:
    GuiColonyGoodsWidget();
    virtual ~GuiColonyGoodsWidget() {}

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
