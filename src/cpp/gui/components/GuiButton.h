#ifndef _GUI_BUTTON_H
#define _GUI_BUTTON_H

#include <SDL.h>
#include <functional>
#include "Context.h"
#include "graphics/graphic/IGraphic.h"
#include "gui/components/GuiStaticElement.h"

/**
 * Grafischer Button, den man einfach nur drücken kann
 */
class GuiButton : public GuiStaticElement {

protected:
    /**
     * @brief Grafik, wenn der Button grade runtergedrückt ist
     */
    const IGraphic* graphicPressed;

    /**
     * @brief gibt an, ob sich der Mauszeiger grade über dem Button befindet
     */
    bool hovered = false;
    
    /**
     * @brief gibt an, ob der Button grade im Zustand "runtergedrückt" steht
     */
    bool pressed = false;

    /**
     * @brief Text, der in der Statusleiste angezeigt wird, wenn mit der Maus über den Button gefahren wird.
     */
    std::string statusBarText;
    
    /**
     * @brief Funktion, die aufgerufen wird, wenn der Button gedrückt wird
     */
    std::function<void()> onClickFunction = []{};

public:
	GuiButton(const Context* const context);
	virtual ~GuiButton() override;

    const IGraphic* getGraphicPressed() const {
        return graphicPressed;
    }

    /**
     * Setzt die Grafik für das Element, wenn der Button gedrückt ist. Der Destruktur räumt die Grafik am Ende weg.
     * @param graphic frisch geladene Grafik, die verwendet werden soll
     */
    void setGraphicPressed(const IGraphic* graphicPressed) {
        this->graphicPressed = graphicPressed;
    }

    void setStatusBarText(const std::string& statusBarText);

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
	virtual void renderElement(IRenderer* renderer) override;
    
    /**
     * @brief Callback, der ein Event handelt
     * @param event SDL-Event
     * @return `true` um das Event an weitere GUI-Elemente zu reichen,
     *         `false` um anzudeuten, dass das Event bereits verarbeitet wurde. Es wird dann nicht weiter zugestellt.
     */
    virtual bool onEventElement(SDL_Event& event) override;

    virtual bool onMouseEnter(SDL_MouseMotionEvent& event) override;
    virtual bool onMouseLeave(SDL_MouseMotionEvent& event) override;

};

#endif
