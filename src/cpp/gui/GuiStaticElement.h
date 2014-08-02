#ifndef _GUI_STATIC_ELEMENT_H
#define _GUI_STATIC_ELEMENT_H

class Graphic;
class SDL_Renderer;


/**
 * Einfaches GUI-Element, was nur aus einer Grafik besteht und nix tut
 */
class GuiStaticElement {

protected:
    /**
     * @brief Position des Element in Fensterkoordinaten
     */
    int windowX, windowY;
    
    /**
     * @brief Grafik, der für das Element gerendert wird
     */
	Graphic* graphic;

public:
	GuiStaticElement();
	~GuiStaticElement();
    
    Graphic* getGraphic() const {
        return graphic;
    }

    void setGraphic(Graphic* graphic) {
        this->graphic = graphic;
    }

    void getWindowCoords(int& windowX, int& windowY) {
        windowX = this->windowX;
        windowY = this->windowY;
    }

    void setWindowCoords(int windowX, int windowY) {
        this->windowX = windowX;
        this->windowY = windowY;
    }
    
	/**
	 * @brief Zeichnet das Element
	 */
	void render(SDL_Renderer* renderer);
    
    /**
     * Callback, der ein Event handelt
     * @param event SDL-Event
     */
    void onEvent(SDL_Event& event) {}

};

#endif
