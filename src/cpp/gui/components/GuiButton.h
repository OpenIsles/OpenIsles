#ifndef _GUI_BUTTON_H
#define _GUI_BUTTON_H

#include <functional>
#include "Context.h"
#include "gui/components/GuiStaticElement.h"

class SDLGraphic;
class SDL_Renderer;


/**
 * Grafischer Button, den man einfach nur drücken kann
 */
class GuiButton : public GuiStaticElement {

protected:
    /**
     * @brief Grafik, wenn der Button grade runtergedrückt ist
     */
    IPlainGraphic* graphicPressed;
    
    /**
     * @brief gibt an, ob der Button grade im Zustand "runtergedrückt" steht
     */
    bool pressed = false;
    
    /**
     * @brief Funktion, die aufgerufen wird, wenn der Button gedrückt wird
     */
    std::function<void()> onClickFunction = []{};

public:
	GuiButton(const Context* const context);
	virtual ~GuiButton();

    IPlainGraphic* getGraphicPressed() const {
        return graphicPressed;
    }

    /**
     * Setzt die Grafik für das Element, wenn der Button gedrückt ist. Der Destruktur räumt die Grafik am Ende weg.
     * @param graphic frisch geladene Grafik, die verwendet werden soll
     */
    void setGraphicPressed(IPlainGraphic* graphicPressed) {
        this->graphicPressed = graphicPressed;
    }
    
    void setOnClickFunction(std::function<void()> onClickFunction) {
        this->onClickFunction = onClickFunction;
    }
    
    /**
     * @brief Ruft die onClick-Funktion auf
     */
    void triggerOnClick() {
        this->onClickFunction();
    }
    
    /**
	 * @brief Zeichnet das Element (ohne Kinder)
	 */
	virtual void renderElement(IRenderer* renderer);
    
    /**
     * @brief Callback, der ein Event handelt
     * @param event SDL-Event
     */
    virtual void onEventElement(SDL_Event& event);

};

#endif
