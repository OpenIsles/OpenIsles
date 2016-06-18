#ifndef _GUI_MAP_H
#define _GUI_MAP_H

#include <SDL_render.h>
#include <memory>
#include <random>
#include <unordered_map>
#include "global.h"
#include "gui/components/map/GuiResourcesBar.h"
#include "gui/components/GuiBase.h"
#include "map/buildqueue/BuildOperation.h"
#include "map/Direction.h"
#include "map/Structure.h"
#include "utils/RectangleData.h"


/**
 * @brief GUI-Element, was die Karte darstellt. Das ist der eigentliche Spielbereich.
 */
class GuiMap : public GuiBase {

private:
    /**
     * @brief Referenz auf die Resourcen-Leiste
     */
    GuiResourcesBar* guiResourcesBar;

    // Datenstrukturen, die für den Baumodus gebraucht werden //////////////////////////////////////////////////////////

    /**
     * Bauoperation, die grade im Gange is
     */
    std::unique_ptr<BuildOperation> buildOperation = nullptr;

    /**
     * @brief `true`, wenn gerade ein neues Gebäude positioniert wird.
     */
    bool inBuildingMode = false;

public:
    /**
     * @brief Konstruktor. Positioniert die Komponente entsprechend `Consts::mapClipRect`.
     * @param context Dependency
     * @param guiResourcesBar Dependency
     */
    GuiMap(const Context& context, GuiResourcesBar* guiResourcesBar);

    virtual ~GuiMap();

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
     * @brief Callback, wenn ein neues Spiel startet.
     */
    void onNewGame();

    /**
     * @brief Callback, wenn der Bau-Modus aktiviert wurde.
     */
    void onStartAddingStructure();

    /**
     * @brief Callback, wenn der Bau-Modus verlassen wird, ohne ein Gebäude zu setzen.
     */
    void onCancelAddingStructure();

    /**
     * @brief Callback, wenn im Bau-Modus das zu positionierende Gebäude gedreht wird.
     * Wir brauchen die Info, um `mapObjectBeingAddedHovering` aktuell zu halten.
     */
    void onRotateAddingStructure();

private:

    // TODO Methoden sortieren!!!

    /**
     * @brief Zeichnet den Einzugsbereich eines zu bauenden Gebäudes.
     * @param renderer (Dependency)
     * @param mapObjectToBuild Gebäude, das gebaut werden soll
     */
    void drawCatchmentArea(IRenderer* const renderer, const MapObjectToBuild& mapObjectToBuild);

    /**
     * @brief Ermittelt, welches klickbares Map-Objekt an einer bestimmten Maus-Position ganz oben liegt.
     *
     * Klickbar sind alle Gebäude und Schiffe.
     *
     * @param mouseX X-Fenster-Koordinate
     * @param mouseY Y-Fenster-Koordinate
     * @return Map-Objekt oder `nullptr`, wenn dort kein klickbares Map-Objekt ist
     */
    const MapObject* getClickableMapObjectUnderMouseCoords(int mouseX, int mouseY);

    /**
     * @brief interner Klickhandler, wenn in die Karte geklickt wurde, um etwas zu selektieren
     * (d.&nbsp;h. nicht im Baumodus).
     *
     * @param mouseX X-Fenster-Koordinate, die geklickt wurde
     * @param mouseY Y-Fenster-Koordinate, die geklickt wurde
     */
    void onClickInMapForSelection(int mouseX, int mouseY);

    /**
     * @brief Löscht `buildOperation` und legt eine frische Instanz an.
     * TODO BUILDOPERATION könnte man hübscher mit BuildOperation.reset() lösen
     */
    void resetBuildOperation();

    /**
    * @brief interner Eventhandler, der beim Loselassen der linken Maustaste im Baumodus greift.
    * Beendet das Setzen neuer Gebäude. Alle Gebäude werden nun gesetzt.
    */
    void onReleaseMouseLeftInBuildingMode();

    /**
     * @brief Aktualisiert die Baukosten in der Resourcen-Leiste.
     */
    void updateBuildingCosts();

    /**
     * @brief Erzeugt/Aktualisiert das Hover-Objekt.
     *
     * Dieser interne Eventhandler wird aufgerufen, wenn
     * - die Maus auf eine andere Kachel bewegt wurde,
     * - wir uns im Baumodus befinden
     * - und die Maustaste NICHT gedrückt ist.
     *
     * Ebenfalls muss diese Methode aufgerufen werden, wenn
     * - in den Baumodus gewechselt wird, damit das Hover-Objekt gezeigt wird oder
     * - der Spieler das Objekt dreht, damit das Hover-Objekt aktualisiert wird.
     */
    void updateHoverObject();

    /**
     * @brief Hilfsfunktion von renderElement(), die eine einzelne Kachel rendert
     * @param mapCoords Map-Koordinate, die zu rendern ist
     */
    void renderTile(const MapCoords& mapCoords);

#ifdef DEBUG_GUIMAP_COORDS
    /**
     * @brief Hilfsfunktion von renderElement(), die die Kachelkoordinaten als Text rendert.
     * Das Rendering sieht nur gut aus, wenn die Zoomstufe auf Maximum ist.
     * (Nur zu Debugzwecken einkompiliert)
     *
     * @param renderer (Dependency)
     * @param map (Dependency)
     * @param mapCoords Map-Koordinate, deren Koordinate zu rendern ist
     */
    void debugRenderTextMapCoords(IRenderer* renderer, const Map& map, const MapCoords& mapCoords);
#endif

    /**
     * Fügt ggf. ein oder mehrere neue Bauaufträge zur `buildOperation` hinzu.
     * Wir tun das wenn,
     * - die linke Maustaste im Baumodus gerade heruntergedrückt wurde
     *   (d.&nbsp;h. der Spieler grade mit dem Bau beginnt)
     * - oder die linke Maustaste bereits gedrückt ist und die Maus auf eine neue Kachel bewegt wurde.
     *
     * @param mustResetBefore (nur relevant, wenn StructurePlacing == INDIVIDUALLY)
     *                        `true`, um `resetBuildOperation()` aufzurufen, bevor die neuen
     *                        Bauaufträge hinzugefügt werden.
     */
    void addToBuildQueration(bool mustResetBefore);

};

#endif
