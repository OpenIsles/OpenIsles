#ifndef _GUI_MGR_H
#define _GUI_MGR_H

#include "SDL.h"

class SDL_Renderer;
class GuiButton;
class GuiStaticElement;


/**
 * GUI-Manager. Verwaltet alle GUI-Elemente.
 */
class GuiMgr {

private:
	GuiStaticElement* panel;
	GuiStaticElement* statusBar;
    GuiButton* testButton;

public:
	GuiMgr();
	~GuiMgr();

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
