#ifndef _GUI_STATIC_ELEMENT_H
#define _GUI_STATIC_ELEMENT_H

#include <SDL.h>
#include "Context.h"
#include "graphics/PlainGraphic.h"
#include "gui/components/GuiBase.h"


/**
 * Einfaches GUI-Element, was nur aus einer Grafik besteht und nix tut
 */
class GuiStaticElement : public GuiBase {

protected: 
    /**
     * @brief Grafik, die für das Element gerendert wird. Kann nullptr sein, wenn keine Grafik gezeichnet werden soll.
     */
    PlainGraphic* graphic = nullptr;

public:
	GuiStaticElement(const Context* const context);
	virtual ~GuiStaticElement();

    PlainGraphic* getGraphic() const {
        return graphic;
    }

    /**
     * Setzt die Grafik für das Element.
     * @param graphic Grafik, die verwendet werden soll
     */
    void setGraphic(PlainGraphic* graphic) {
        this->graphic = graphic;
    }
    
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
