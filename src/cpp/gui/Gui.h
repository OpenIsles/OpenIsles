#ifndef _GUI_H
#define _GUI_H

#include "SDL.h"
#include "Graphic.h"

class Gui {

private:
	Graphic* panel;
	Graphic* statusBar;

public:
	Gui();
	~Gui();

	/**
	 * @brief Zeichnet die Benutzeroberfläche
	 */
	void renderGui(SDL_Renderer* renderer);

};

#endif
