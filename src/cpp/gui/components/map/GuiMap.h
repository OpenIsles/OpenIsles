#ifndef _GUI_MAP_H
#define _GUI_MAP_H

#include <SDL_render.h>
#include <memory>
#include <random>
#include <unordered_map>
#include "Context.h"
#include "gui/components/map/GuiResourcesBar.h"
#include "gui/components/GuiBase.h"
#include "map/buildqueue/BuildOperation.h"
#include "map/Direction.h"
#include "map/Structure.h"
#include "utils/RectangleData.h"

#if defined(DEBUG_GUIMAP_COORDS) || defined(DEBUG_GUIMAP)
#include "graphics/graphic/sdl/SDLGraphic.h"
#endif


// Konstanten für isAllowedToPlaceMapObject()

/**
 * @brief Setzen der Struktur ist erlaubt
 */
#define PLACING_STRUCTURE_ALLOWED                 0

/**
 * @brief Setzen der Struktur ist nicht erlaubt wegen Resourcen-Mangel.
 * Grafisch wird dies dargestellt, indem das Gebäude blickt.
 */
#define PLACING_STRUCTURE_NO_RESOURCES                       (1 << 0)

/**
 * @brief Setzen der Struktur ist hier nicht erlaubt, weil was im Weg ist.
 */
#define PLACING_STRUCTURE_SOMETHING_IN_THE_WAY               (1 << 1)

/**
 * @brief (nur während Baumodus:) Setzen der Struktur ist hier nicht mehr erlaubt,
 * weil bereits ein anderes Objekt dort gebaut werden soll.
 */
#define PLACING_STRUCTURE_SOMETHING_IN_THE_WAY_TEMPORARILY   (1 << 2)

/**
 * @brief Setzen der Struktur ist hier nicht erlaubt, weil das Gebiet nicht erschlossen ist.
 */
#define PLACING_STRUCTURE_ROOM_NOT_UNLOCK                    (1 << 3)

/**
 * @brief Setzen der Struktur ist hier nicht erlaubt, weil das Gelände nicht passt.
 */
#define PLACING_STRUCTURE_MAP_TILE_TYPE_MISMATCH             (1 << 4)

/**
 * @brief Setzen der Struktur ist nicht erlaubt, da wir mitten auf dem Ozean außerhalb einer Insel sind.
 * Grafisch wird dies dargestellt, indem gar nix gezeichnet wird.
 */
#define PLACING_STRUCTURE_OUTSIDE_OF_ISLE                    (1 << 5)


/**
 * @brief Ähnlich `MapTile` stellt diese Struktur eine Kachel auf der Karte dar. Hier sind alle Infos drin, die wir
 * temporär brauchen. Wir müssen sie zwar zeichnen, allerdings sind sie noch nicht im Spielstand enthalten.
 *
 * Außerdem sind hier bevorstehende Veränderungen enthalten. Beispiele:
 * - Eine Waldkachel muss temporär gezeichnet werden (als Hover-Objekt bezeichnet).
 *   Sie ist aktuell nicht gesetzt, da der Spieler noch nicht die Maustaste gedrückt hat.
 * - Eine Straßen-Kurve-Kachel muss temporär durch eine T-Kachel ersetzt werden, wenn eine Straße angeschlossen wird.
 *   Auch diese Veränderung muss bei Bauabschluss in den Spielzustand übernommen werden.
 * - Temporär muss eine Kachel maskiert gezeichnet werden.
 *   Bei Bauabschluss muss die Kachel gelöscht werden, da der Nutzer das Abrisswerkzeug benutzt hat.
 * - Temporär wird statt einer Waldkachel ein Wohnhaus gezeichnet. Hier ist zu beachten, dass das Wohnhaus 2x2 Felder
 *   groß ist, die Waldkachel nur 1x1. Alle 4 Kacheln erhalten eine temporäre `draw`-Ersetzung. Außerdings ist die
 *   `build`-Ersetzung nur bei einer Kachel das Wohnhaus. Die anderen 3 Kacheln sind auf `nullptr` gesetzt.
 */
struct MapTileTemporarily {

public:
    /**
     * @brief Zeiger auf ein MapObject, das auf dieser Kachel gezeichnet werden soll.
     * `nullptr` bedeutet, keine Ersetzung beim Zeichnen.
     * Dasselbe Objekt wird via shared_ptr mehrfach in die Map eingebunden, wenn es mehrere Kacheln belegt.
     */
    std::shared_ptr<MapObjectFixed> mapObjectToDraw = nullptr;

    /**
     * @brief wenn auf `true` gesetzt, wird die Kachel maskiert gezeichnet.
     */
    bool drawTileMasked = false;
};


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


#if defined(DEBUG_GUIMAP) || defined(DEBUG_GUIMAP_COORDS)
    /**
     * @brief Hilfsgrafiken mit Gitternetz-Linien-Overlays
     *
     * 0 = Gitternetz-Linien-Overlay für elevation 0
     * 1 = Gitternetz-Linien-Overlay für elevation 1
     * 2 = Achsen für ScreenCoords
     */
    SDLGraphic* debugGridOverlayGraphics[3];
#endif

    /**
     * @brief Referenz auf das Baumaterial "Werkzeug"
     * TODO wenn wir alles konfigurierbar machen wollen, sollte das später auch mal weg und die Baumaterialen dynamisch werden
     */
    const Good* toolsGood;

    /**
     * @brief Referenz auf das Baumaterial "Holz"
     * TODO wenn wir alles konfigurierbar machen wollen, sollte das später auch mal weg und die Baumaterialen dynamisch werden
     */
    const Good* woodGood;

    /**
     * @brief Referenz auf das Baumaterial "Ziegel"
     * TODO wenn wir alles konfigurierbar machen wollen, sollte das später auch mal weg und die Baumaterialen dynamisch werden
     */
    const Good* bricksGood;

public:
    /**
     * @brief Konstruktor. Positioniert die Komponente entsprechend `Consts::mapClipRect`.
     * @param context Dependency
     * @param guiResourcesBar Dependency
     */
    GuiMap(const Context* const context, GuiResourcesBar* guiResourcesBar);

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
     * @brief Ermittelt, welches Map-Objekt (MapObjectFixed) an einer bestimmten Maus-Position ganz oben liegt.
     * @param mouseX X-Fenster-Koordinate
     * @param mouseY Y-Fenster-Koordinate
     * @return Map-Objekt oder `nullptr`, wenn dort kein Map-Objekt ist
     */
    const MapObjectFixed* getMapObjectFixedUnderMouseCoords(int mouseX, int mouseY);

    /**
     * @brief interner Klickhandler, wenn in die Karte geklickt wurde, um etwas zu selektieren
     * (d.&nbsp;h. nicht im Baumodus).
     *
     * @param mouseX X-Fenster-Koordinate, die geklickt wurde
     * @param mouseY Y-Fenster-Koordinate, die geklickt wurde
     */
    void onClickInMapForSelection(int mouseX, int mouseY);

    /**
     * Prüft, ob eine bestimmte Struktur an eine bestimmte Position gesetzt werden darf.
     *
     * @param mapCoords Map-Koordinate, wo die Struktur gesetzt werden soll
     * @param mapObjectType Typ der zu setzenden Struktur
     * @param view Ausrichtung der Struktur
     * @sa PLACING_STRUCTURE-Konstanten
     * @return Bitmaske, die angibt, ob das Gebäude gesetzt werden darf.
     */
    unsigned char isAllowedToPlaceMapObject(
        const MapCoords& mapCoords, const MapObjectType* mapObjectType, const FourthDirection& view) const;

    /**
     * @brief Löscht `buildOperation` und legt eine frische Instanz an.
     * TODO BUILDOPERATION könnte man hübscher mit BuildOperation.reset() lösen
     */
    void resetBuildOperation();

    /**
     * @brief Entfernt alle Objekte in `buildQueue` und `mapTilesToDrawTemporarily` und setzt
     * `buildingToDrawCatchmentArea` auf `nullptr`.
     */
    void clearAllTemporarily();

    /**
    * @brief interner Eventhandler, der beim Loselassen der linken Maustaste im Baumodus greift.
    * Beendet das Setzen neuer Gebäude. Alle Gebäude werden nun gesetzt.
    */
    void onReleaseMouseLeftInBuildingMode();

    /**
     * @brief Fügt ein Gebäude zu `buildQueue` hinzu.
     * Der Aufrufer muss sich sicher sein, dass das Gebäude hier platziert werden darf.
     *
     * @param mapCoords Map-Koordinaten, wo das Gebäude platziert werden soll
     */
    void addCurrentStructureToBuildQueue(const MapCoords& mapCoords);

    /**
     * @brief Aktualisiert die `drawingFlags` der `mapTilesToDrawTemporarily`.
     * Dies ist immer vor jedem Frame-Rendern notwendig, da sich ständig die verfügbaren Resourcen ändern.
     */
    void updateMapObjectsTemporarilyDrawingFlags();

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

//    /**
//     * @brief Aktualisiert die `mapTilesToDrawTemporarily` einer bestimmten. Dies verändert ggf. eine
//     * Straßenkachel, wenn wir im Begriff sind, Straßen zu bauen und somit z.&nbsp;B. eine Gerade zu einem
//     * T-Stück wird.
//     *
//     * @param mapCoordsToUpdate Map-Koordinate, die ggf. aktualisiert werden soll
//     */
//    void updateMapTilesToDrawTemporarilyForStreetsAt(const MapCoords& mapCoordsToUpdate);
//
//    /**
//     * @brief Aktualisiert die `mapTilesToDrawTemporarily` um eine bestimmte Kachel herum. Dies verändert ggf.
//     * Straßenkacheln, wenn wir im Begriff sind, Straßen zu bauen und somit z.&nbsp;B. eine Gerade zu einem
//     * T-Stück wird.
//     *
//     * @param mapCoordsToUpdateAround Map-Koordinate, um die horizontal- und vertikal-angrenzende Felder ggf.
//     * aktualisiert werden sollen
//     */
//    void updateMapTilesToDrawTemporarilyForStreetsAround(const MapCoords& mapCoordsToUpdateAround);

};

#endif
