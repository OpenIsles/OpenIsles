#ifndef _GUI_BASE_H
#define _GUI_BASE_H

#include <list>
#include "Context.h"

union SDL_Event;
class SDL_Renderer;


/**
 * Basisklasse für alle GUI-Elemente
 */
class GuiBase : public ContextAware {

    friend class GuiMgr; // guiMgr.registerElement() setzt id

private:
    /**
     * ID mit welcher das GUI-Element registriert ist. Der GuiMgr setzt es einmalig, wenn guiMgr.registerElement()
     * aufgerufen wird. Es sollte nicht von Hand geändert werden
     */
    int id;

protected:
    /**
     * @brief Position des Elements in Pixelkoordinaten. Ist das Element ein Child-Element eines anderen Elements,
     * so sind die Koordinaten relativ zu diesem Parent, ansonsten sind es Fensterkoordinaten.
     */
    int x, y;
    
    /**
     * @brief Größe des Elements.
     */
    int width, height;
    
    /**
     * @brief gibt an, ob das Element sichtbar ist, d.h. gerendert wird und Events empfangen kann.
     */
    bool visible;
    
    /**
     * @brief Liste von Kind-Elementen
     */
    std::list<GuiBase*> childElements;
    
    /**
     * @brief Parent-Element. Wenn nicht gesetzt, beziehen sich die Angaben x und y auf das Fenster, sonst
     * auf das Parent-Element.
     * Diese Eigenschaft wird von addChildElement() gesetzt. Niemals von Hand umsetzen.
     */
    GuiBase* parentElement = nullptr;

public:
	GuiBase(const Context* const context) : ContextAware(context) {
        visible = true;
    }
	virtual ~GuiBase() {}

    int getId() const {
        return id;
    }

    void getCoords(int& x, int& y, int& width, int& height) {
        x = this->x;
        y = this->y;
        width = this->width;
        height = this->height;
    }

    void setCoords(int x, int y, int width, int height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }
    
    /**
     * @brief Prüft, ob die angegebenen Fenster-Koordinaten innerhalb des Elements liegen.
     * @param windowX X-Fenster-Koordinate
     * @param windowY Y-Fenster-Koordinate
     * @return true, wenn innerhalb des Elements; sonst false
     */
    bool hitTest(int windowX, int windowY) {
        // Fensterkoordinaten ermitteln
        int thisWindowX, thisWindowY;
        getWindowCoords(thisWindowX, thisWindowY);
        
        // Kollision prüfen
        return ((windowX >= thisWindowX) && (windowY >= thisWindowY) &&
                (windowX < thisWindowX + width) && (windowY < thisWindowY + height));
    }
    
    bool isVisible() const {
        return visible;
    }

    void setVisible(bool visible) {
        this->visible = visible;
    }
    
    GuiBase* const getParentElement() const {
        return parentElement;
    }
    
    /**
     * @brief Fügt ein neues Child-Element hinzu. parentElement wird automatisch gesetzt.
     * @param childElement Child-Element
     */
    void addChildElement(GuiBase* childElement) {
        childElement->parentElement = this;
        childElements.push_front(childElement);
    }
    
    /**
	 * @brief Zeichnet das Element und rekursiv seine Kinder
	 */
    void render(IRenderer* renderer) {
        if (!visible) {
            return;
        }
        
        renderElement(renderer);
        
        for (auto iter = childElements.cbegin(); iter != childElements.cend(); iter++) {
            (*iter)->render(renderer);
        }
    }
    
    /**
     * @brief Callback, der ein Event handelt. Das Event wird rekursiv an alle Kinder übertragen
     * @param event SDL-Event
     */
    void onEvent(SDL_Event& event) {
        if (!visible) {
            return;
        }
        
        onEventElement(event);
        
        for (auto iter = childElements.cbegin(); iter != childElements.cend(); iter++) {
            (*iter)->onEvent(event);
        }
    }
    
	/**
	 * @brief Zeichnet das Element (ohne Kinder)
	 */
	virtual void renderElement(IRenderer* renderer) = 0;
    
    /**
     * @brief Callback, der ein Event handelt
     * @param event SDL-Event
     */
    virtual void onEventElement(SDL_Event& event) = 0;
    
protected:
    /**
     * @brief Ermittelt die Fensterkoordinaten des Elements, d.h. die Position, wo das Element effektiv im Fenster
     * liegt.
     * @param windowX erhält die X-Fensterkoordinate
     * @param windowY erhält die Y-Fensterkoordinate
     */
    void getWindowCoords(int& windowX, int& windowY) {
        if (parentElement == nullptr) {
            windowX = x;
            windowY = y;
        } else {
            parentElement->getWindowCoords(windowX, windowY);
            windowX += x;
            windowY += y;
        }
    }

};

#endif
