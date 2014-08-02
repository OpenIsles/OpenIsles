#ifndef _GUI_BUTTON_H
#define _GUI_BUTTON_H

#include "GuiStaticElement.h"

class Graphic;
class SDL_Renderer;


/**
 * Grafischer Button
 */
class GuiButton : public GuiStaticElement {

private:
    /**
     * @brief Grafik, wenn der Button gedrückt ist
     */
	Graphic* graphicPressed;
    
    /**
     * @brief gibt an, ob der Button im Zustand "gedrückt" steht
     */
    bool pressed = false;

public:
	GuiButton();
	~GuiButton();
    
    Graphic* getGraphicPressed() const {
        return graphicPressed;
    }

    void setGraphicPressed(Graphic* graphicPressed) {
        this->graphicPressed = graphicPressed;
    }
    
    /**
	 * @brief Zeichnet das Element
	 */
	void render(SDL_Renderer* renderer);
    
    /**
     * Callback, der ein Event handelt
     * @param event SDL-Event
     */
    void onEvent(SDL_Event& event);

};

#endif
