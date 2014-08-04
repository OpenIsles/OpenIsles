#ifndef _GUI_BASE_H
#define _GUI_BASE_H

union SDL_Event;
class SDL_Renderer;


/**
 * Basisklasse für alle GUI-Elemente
 */
class GuiBase {

protected: 
    /**
     * @brief Position des Element in Fensterkoordinaten
     */
    int windowX, windowY;
    
    /**
     * @brief Größe des Elements. Die Grafiken müssen dieselbe Größe haben!
     */
    int width, height;

public:
	GuiBase() {}
	virtual ~GuiBase() {}

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
	virtual void render(SDL_Renderer* renderer) = 0;
    
    /**
     * Callback, der ein Event handelt
     * @param event SDL-Event
     */
    virtual void onEvent(SDL_Event& event) = 0;

};

#endif
