#ifndef _GUI_BUTTON_H
#define _GUI_BUTTON_H

#include "GuiStaticElement.h"

class Graphic;
class SDL_Renderer;


/**
 * Grafischer Button, den man einfach nur drücken kann
 */
class GuiButton : public GuiStaticElement {

protected:
    /**
     * @brief Grafik, wenn der Button grade runtergedrückt ist
     */
	Graphic* graphicPressed;
    
    /**
     * @brief gibt an, ob der Button grade im Zustand "runtergedrückt" steht
     */
    bool pressed = false;

public:
	GuiButton();
	virtual ~GuiButton();
    
    Graphic* getGraphicPressed() const {
        return graphicPressed;
    }

    void setGraphicPressed(Graphic* graphicPressed) {
        this->graphicPressed = graphicPressed;
    }
    
    /**
	 * @brief Zeichnet das Element
	 */
	virtual void render(SDL_Renderer* renderer);
    
    /**
     * Callback, der ein Event handelt
     * @param event SDL-Event
     */
    virtual void onEvent(SDL_Event& event);

};

#endif
