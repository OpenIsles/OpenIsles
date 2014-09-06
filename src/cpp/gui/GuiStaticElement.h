#ifndef _GUI_STATIC_ELEMENT_H
#define _GUI_STATIC_ELEMENT_H

#include "GuiBase.h"

union SDL_Event;
class Graphic;
class SDL_Renderer;


/**
 * Einfaches GUI-Element, was nur aus einer Grafik besteht und nix tut
 */
class GuiStaticElement : public GuiBase {

protected: 
    /**
     * @brief Grafik, der für das Element gerendert wird. Kann nullptr sein, wenn keine Grafik gezeichnet werden soll.
     */
	Graphic* graphic = nullptr;

public:
	GuiStaticElement();
	virtual ~GuiStaticElement();
    
    Graphic* getGraphic() const {
        return graphic;
    }

    /**
     * Setzt die Grafik für das Element. Der Destruktur räumt die Grafik am Ende weg.
     * @param graphic frisch geladene Grafik, die verwendet werden soll
     */
    void setGraphic(Graphic* graphic) {
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
