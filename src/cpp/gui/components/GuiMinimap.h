#ifndef _GUI_MINIMAP_H
#define _GUI_MINIMAP_H

#include <SDL_render.h>
#include "global.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "map/Structure.h"
#include "utils/Rect.h"


/**
 * @brief GUI-Element, was die Minimap darstellt.
 */
class GuiMinimap : public GuiStaticGraphicElement {

private:
    /**
     * @brief relative Koordinaten (zum GUI-Element) des Clip-Rechtecks der Minimap.
     * 
     * Das ist die Fläche im GUI-Element, in welchen
     * - etwas über das Hintergrundbild gezeichnet wird
     * - Klicks reagieren
     * - geclippt wird (Wir wollen nicht, dass das Rechteck, was den aktuellen Kartenausschnitt markiert,
     * zu weit raus geht)
     */
    Rect guiElementBasedClipRect;

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
    GuiMinimap(const Context& context);

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
