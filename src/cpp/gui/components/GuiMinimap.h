#ifndef _GUI_MINIMAP_H
#define _GUI_MINIMAP_H

#include <SDL_render.h>
#include "Context.h"
#include "gui/components/GuiBase.h"
#include "map/Structure.h"
#include "utils/RectangleData.h"


/**
 * @brief GUI-Element, was die Minimap darstellt.
 */
class GuiMinimap : public GuiBase {

private:
    /**
     * @brief SDL-Texture mit der Minimap. Diese wird vorberechnet und hier gespeichert, sodass sie nur gezeichnet
     * werden muss und keine Berechnungszeit anfällt.
     */
    SDL_Texture* minimapTexture = nullptr;

public:
    /**
     * @brief Konstruktor. Positioniert die Komponente entsprechend `Consts::minimapClipRect`.
     * @param context Dependency
     */
    GuiMinimap(const Context* const context);

    virtual ~GuiMinimap();

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);

    /**
     * @brief Callback, der ein Event handelt
     * @param event SDL-Event
     */
    virtual void onEventElement(SDL_Event& event);

    /**
     * @brief Muss aufgerufen werden, wenn sich die Einzugsgebiete von Lagergebäuden geändert haben. Die Komponente
     * aktualisiert damit ihre `minimapTexture`.
     */
    void updateMinimapTexture();

private:
    /**
     * @brief interner Klickhandler, wenn in die Minimap geklickt wurde.
     * @param mouseX X-Fenster-Koordinate, die geklickt wurde
     * @param mouseY Y-Fenster-Koordinate, die geklickt wurde
     */
    void onClickInMinimap(int mouseX, int mouseY);

};

#endif
