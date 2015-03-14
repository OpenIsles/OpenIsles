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
    // Die folgenden Daten werden vorberechnet hier gespeichert, sodass nur gezeichnet werden müssen und keine
    // Berechnungszeit pro Frame anfällt.

    /**
     * @brief SDL-Texture mit der Minimap.
     */
    SDL_Texture* minimapTexture = nullptr;

    /**
     * @brief `SDL_Point`-Linienzug, der den aktuellen Ausschnitt der Karte enthält.
     */
    SDL_Point pointsCurrentClipping[5];

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
     * @return `true` um das Event an weitere GUI-Elemente zu reichen,
     *         `false` um anzudeuten, dass das Event bereits verarbeitet wurde. Es wird dann nicht weiter zugestellt.
     */
    virtual bool onEventElement(SDL_Event& event);

    /**
     * @brief Muss aufgerufen werden, wenn sich der aktuelle Ausschnitt (z,&nbsp;B. durch Scrolling oder Ändern der
     * Zoomstufe) geändert habt. Die Komponente aktualisiert damit ihre `pointsCurrentClipping`.
     */
    void onMapCoordsChanged();

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
