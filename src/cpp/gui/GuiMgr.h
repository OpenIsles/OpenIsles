#ifndef _GUI_MGR_H
#define _GUI_MGR_H

#include <map>
#include "SDL.h"

class GuiBase;
class GuiButton;
class GuiStaticElement;
class GuiPushButton;
class SDL_Renderer;


/**
 * GUI-Manager. Verwaltet alle GUI-Elemente.
 */
class GuiMgr {

private:
	/**
     * @brief Fenster-Koordinaten, an dem der letzte Click gestartet wurde.
     * Gültig, bis die Maustaste losgelassen wurde.
     */
    int startClickX, startClickY;
    
    /**
     * @brief Map, die alle registrierten Gui-Elemente enthält und durch ihre Identifier referenziert.
     */
    std::map<int, GuiBase*> identifierMap;

public:
	GuiMgr();
	~GuiMgr();
    
    void getStartClickCoords(int& startClickX, int& startClickY) {
        startClickX = this->startClickX;
        startClickY = this->startClickY;
    }

	/**
	 * @brief Zeichnet die Benutzeroberfläche
	 */
	void render(SDL_Renderer* renderer);
    
    /**
     * Callback, der ein Event handelt
     * @param event SDL-Event
     */
    void onEvent(SDL_Event& event);
    
private:
    /**
     * @brief Registriert ein neues GUI-Element
     * @param identifier eindeutiger Identifier, um das Element mit diesem anzusprechen
     * @param guiElement Referenz zum GUI-Element
     */
    void registerElement(int identifier, GuiBase* guiElement);
    
    /**
     * @brief Liefert ein bestimmtes GUI-Element zurück.
     * @param identifier Identifier des Elements
     * @return Referenz zum GUI-Element
     */
    GuiBase* findElement(int identifier);

};

#endif
