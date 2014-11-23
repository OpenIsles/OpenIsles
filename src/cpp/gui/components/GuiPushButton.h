#ifndef _GUI_PUSH_BUTTON_H
#define _GUI_PUSH_BUTTON_H

#include "gui/components/GuiButton.h"

class SDL_Renderer;


/**
 * Button, den man an- und ausschalten kann.
 * Dieser Button nutzt für "heruntergedrückt" und "angeschaltet" dieselben Grafiken.
 */
class GuiPushButton : public GuiButton {

private:
    /**
     * @brief gibt an, ob der Button an- (true) oder ausgeschaltet (false) ist.
     */
    bool active = false;

public:
	GuiPushButton();
	virtual ~GuiPushButton();
    
    /**
     * @return true, wenn der Button angeschaltet ist, sonst false
     */
    bool isActive() {
        return active;
    }
    
    /**
     * @brief Setzt den Zustand des Buttons
     * @param active true, um den Button anzuschalten, false, um ihn auszuschalten
     */
    void setActive(bool active) {
        this->active = active;
    }
    
    /**
	 * @brief Zeichnet das Element (ohne Kinder)
	 */
	virtual void renderElement(SDL_Renderer* renderer);
    
    /**
     * @brief Callback, der ein Event handelt
     * @param event SDL-Event
     */
    virtual void onEventElement(SDL_Event& event);

};

#endif
