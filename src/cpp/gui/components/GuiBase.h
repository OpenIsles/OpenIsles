#ifndef _GUI_BASE_H
#define _GUI_BASE_H

#include <SDL.h>
#include <list>
#include <string>
#include "global.h"
#include "graphics/renderer/IRenderer.h"

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
    int id = 0;

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

    /**
     * @brief gibt an, ob sich der Mauszeiger grade über dem Element befindet
     */
    bool hovered = false;

    /**
     * @brief Text, der in der Statusleiste angezeigt wird, wenn mit der Maus über das Element gefahren wird.
     */
    std::string statusBarText;

public:
    GuiBase(const Context& context) : ContextAware(context) {
        visible = true;
        x = y = 0;
        width = height = 0;
    }
    virtual ~GuiBase() {}

    int getId() const {
        return id;
    }

    int getX() const {
        return x;
    }

    void setX(int x) {
        this->x = x;
    }

    int getY() const {
        return y;
    }

    void setY(int y) {
        this->y = y;
    }

    int getWidth() const {
        return width;
    }

    void setWidth(int width) {
        this->width = width;
    }

    int getHeight() const {
        return height;
    }

    void setHeight(int height) {
        this->height = height;
    }

    void getCoords(int& x, int& y, int& width, int& height) const {
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

    void setPosition(int x, int y) {
        this->x = x;
        this->y = y;
    }
    
    /**
     * @brief Prüft, ob die angegebenen Fenster-Koordinaten innerhalb des Elements liegen.
     * @param windowX X-Fenster-Koordinate
     * @param windowY Y-Fenster-Koordinate
     * @return true, wenn innerhalb des Elements; sonst false
     */
    bool hitTest(int windowX, int windowY) const {
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
     * @brief Entfernt ein Child-Element. parentElement wird hierbei auf `nullptr` gesetzt.
     * @param childElement Child-Element
     */
    void removeChildElement(GuiBase* childElement) {
        childElements.remove(childElement);
        childElement->parentElement = nullptr;
    }

    /**
     * @brief Setzt den Text, der in der Statusleiste angezeigt wird, wenn mit der Maus über das Element gefahren wird.
     * @param statusBarText anzuzeigender Text in der Statusleiste
     */
    void setStatusBarText(const std::string& statusBarText);
    
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
     * @brief Callback, der ein Event handelt. Das Event wird rekursiv an alle Kinder übertragen.
     *
     * Diese Implementierung erzeugt die Events onMouseMove, onMouseEnter und onMouseLeave.
     * Außerdem setzt die Variable hovered und kümmert sich drum, den Statusleisten-Text zu setzen, wenn
     * mit der Maus über das Element gefahren wird.
     *
     * @param event SDL-Event
     * @return `true` um das Event an weitere GUI-Elemente zu reichen,
     *         `false` um anzudeuten, dass das Event bereits verarbeitet wurde. Es wird dann nicht weiter zugestellt.
     */
    bool onEvent(SDL_Event& event);
    
    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) = 0;
    
    /**
     * @brief Callback, der aufgerufen wird, wenn irgendein Event eingeht.
     * 
     * Unterklassen können diese Methode überschreiben und das Event verwenden.
     * Default-Implementierung: Nichts tun und Event weitergeben.
     * 
     * @param event SDL-Event
     * @return `true` um das Event an weitere GUI-Elemente zu reichen,
     *         `false` um anzudeuten, dass das Event bereits verarbeitet wurde. Es wird dann nicht weiter zugestellt.
     */
    virtual bool onEventElement(SDL_Event& event) {
        return true;
    }

    /**
     * @brief Callback, der aufgerufen wird, wenn der Mauszeiger in die GUI-Komponente hineinbewegt wird.
     * 
     * Unterklassen können diese Methode überschreiben und das Event verwenden.
     * Default-Implementierung: Nichts tun und Event weitergeben.
     * 
     * @param event SDL-Event
     * @return `true` um das Event an weitere GUI-Elemente zu reichen,
     *         `false` um anzudeuten, dass das Event bereits verarbeitet wurde. Es wird dann nicht weiter zugestellt.
     */
    virtual bool onMouseEnter(SDL_MouseMotionEvent& event) { 
        return true; 
    }
    
    /**
     * @brief Callback, der aufgerufen wird, wenn der Mauszeiger aus der GUI-Komponente herausbewegt wird.
     * 
     * Unterklassen können diese Methode überschreiben und das Event verwenden.
     * Default-Implementierung: Nichts tun und Event weitergeben.
     * 
     * @param event SDL-Event
     * @return `true` um das Event an weitere GUI-Elemente zu reichen,
     *         `false` um anzudeuten, dass das Event bereits verarbeitet wurde. Es wird dann nicht weiter zugestellt.
     */
    virtual bool onMouseLeave(SDL_MouseMotionEvent& event) {
        return true;
    }
    
    /**
     * @brief Callback, der aufgerufen wird, wenn der Mauszeiger in der GUI-Komponente bewegt wird.
     * 
     * Unterklassen können diese Methode überschreiben und das Event verwenden.
     * Default-Implementierung: Nichts tun und Event weitergeben.
     *
     * Hinweis: Bei onMouseEnter() und onMouseLeave() wird zusätzlich immer vorher onMouseMove() gefeuert.
     * 
     * @param event SDL-Event
     * @return `true` um das Event an weitere GUI-Elemente zu reichen,
     *         `false` um anzudeuten, dass das Event bereits verarbeitet wurde. Es wird dann nicht weiter zugestellt.
     */
    virtual bool onMouseMove(SDL_MouseMotionEvent& event) {
        return true;
    }

    /**
     * @brief Ermittelt die Fensterkoordinaten des Elements, d.h. die Position, wo das Element effektiv im Fenster
     * liegt.
     * @param windowX erhält die X-Fensterkoordinate
     * @param windowY erhält die Y-Fensterkoordinate
     */
    void getWindowCoords(int& windowX, int& windowY) const {
        if (parentElement == nullptr) {
            windowX = x;
            windowY = y;
        } else {
            parentElement->getWindowCoords(windowX, windowY);
            windowX += x;
            windowY += y;
        }
    }

    int getWindowX() const {
        int windowX, windowY;
        getWindowCoords(windowX, windowY);

        return windowX;
    }

    int getWindowY() const {
        int windowX, windowY;
        getWindowCoords(windowX, windowY);

        return windowY;
    }

};

#endif
