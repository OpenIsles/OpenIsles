#ifndef _GUI_STATIC_ELEMENT_H
#define _GUI_STATIC_ELEMENT_H

union SDL_Event;
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
     * @brief Größe des Elements. Die Grafiken müssen dieselbe Größe haben!
     */
    int width, height;
    
    /**
     * @brief Grafik, der für das Element gerendert wird
     */
	Graphic* graphic;

public:
	GuiStaticElement();
	virtual ~GuiStaticElement();
    
    Graphic* getGraphic() const {
        return graphic;
    }

    void setGraphic(Graphic* graphic) {
        this->graphic = graphic;
    }

    void getWindowCoords(int& windowX, int& windowY, int& width, int& height) {
        windowX = this->windowX;
        windowY = this->windowY;
        width = this->width;
        height = this->height;
    }

    void setWindowCoords(int windowX, int windowY, int width, int height) {
        this->windowX = windowX;
        this->windowY = windowY;
        this->width = width;
        this->height = height;
    }
    
    /**
     * Prüft, ob die angegebenen Fenster-Koordinaten innerhlab des Elements liegen.
     * @param x X-Fenster-Koordinate
     * @param y Y-Fenster-Koordinate
     * @return true, wenn innerhalb des Elements; sonst false
     */
    bool hitTest(int x, int y) {
        return (x >= windowX && y >= windowY && x < windowX + width && y < windowY + height);
    }
    
	/**
	 * @brief Zeichnet das Element
	 */
	virtual void render(SDL_Renderer* renderer);
    
    /**
     * Callback, der ein Event handelt
     * @param event SDL-Event
     */
    virtual void onEvent(SDL_Event& event) {
        // nix tun
    }

};

#endif
