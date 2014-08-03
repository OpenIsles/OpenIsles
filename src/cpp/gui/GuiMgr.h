#ifndef _GUI_MGR_H
#define _GUI_MGR_H

#include "SDL.h"

class GuiButton;
class GuiStaticElement;
class GuiPushButton;
class SDL_Renderer;


/**
 * GUI-Manager. Verwaltet alle GUI-Elemente.
 */
class GuiMgr {

private:
	GuiStaticElement* panel;
	GuiStaticElement* statusBar;
    GuiPushButton* musicPushButton;
    
    GuiButton* testButton;
    GuiPushButton* testPushButton;
    
    /**
     * @brief Fenster-Koordinaten, an dem der letzte Click gestartet wurde.
     * Gültig, bis die Maustaste losgelassen wurde.
     */
    int startClickX, startClickY;

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

};

#endif
