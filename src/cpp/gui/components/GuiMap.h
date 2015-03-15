#ifndef _GUI_MAP_H
#define _GUI_MAP_H

#include <SDL_render.h>
#include "Context.h"
#include "gui/components/GuiBase.h"
#include "map/Direction.h"
#include "map/Structure.h"
#include "utils/RectangleData.h"

#ifdef DEBUG
#include "graphics/graphic/sdl/SDLGraphic.h"
#endif


// Konstanten für isAllowedToPlaceStructure()

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
 * Grafisch wird dies dargestellt, indem die Struktur rot gezeichnet wird
 */
#define PLACING_STRUCTURE_ROOM_NOT_UNLOCK                    (1 << 3)

/**
 * @brief Setzen der Struktur ist nicht erlaubt, da wir mitten auf dem Ozean außerhalb einer Insel sind.
 * Grafisch wird dies dargestellt, indem gar nix gezeichnet wird.
 */
#define PLACING_STRUCTURE_OUTSIDE_OF_ISLE                    (1 << 4)


/**
 * @brief Ähnlich `MapTile` stellt diese Struktur eine Kachel auf der Karte dar. Hier sind alle Infos drin, die wir
 * zwar zeichnen müssen, allerdings noch nicht im Spielstand enthalten sind.
 */
struct MapTileTemporialy {

public:
    /**
     * @brief Zeiger auf ein MapObject (durch `mapObjectsBeingAdded` verwaltet), das auf dieser Kachel gezeichnet
     * werden soll oder nullptr, wenns nix da is.
     */
    MapObjectFixed* mapObject = nullptr;
};


/**
 * @brief GUI-Element, was die Karte darstellt. Das ist der eigentliche Spielbereich.
 */
class GuiMap : public GuiBase {

private:
    // Datenstrukturen, die für den Baumodus gebraucht werden //////////////////////////////////////////////////////////

    /**
     * @brief `true`, wenn gerade ein neues Gebäude positioniert wird.
     */
    bool inBuildingMode = false;

    /**
     * Liste von Map-Objekten, die gerade hinzugefügt werden. Die Reihenfolge ist die, in der der Spieler sie
     * hinzugefügt hat. Im Falle von Resourcenmangel werden nur vordersten Objekte wirklich gesetzt.
     * In dieser Liste sind alle Gebäude, die beim Loslassen der linken Maustaste wirklich gesetzt werden. D.&nbsp;h.
     * `mapObjectBeingAddedHovering` befindet sich NICHT in dieser Liste
     *
     * Die `drawingFlags` dieser Objekte sind nicht immer korrekt, da sich die verfügbaren Resourcen ständig ändern.
     * Sie werden bei jedem Frame vor dem Zeichnen aktualisiert.
     *
     * TODO später mit einem Zusatzflag, ob sie nicht grade abgerissen werden
     */
    std::list<Structure*> mapObjectsBeingAdded;

    /**
     * @brief Zeiger auf ein Gebäude (selbst verwaltet), was gerade positioniert wird. Es befindet sich nicht in
     * `mapObjectsBeingAdded`, da noch unklar ist, wo der Spieler es hinsetzen will.
     *
     * Wir haben hier immer genau dann einen Wert gesetzt, wenn `inBuildingMode == true` und der Spieler noch nicht
     * die linke Maustaste gedrückt hat.
     */
    Structure* mapObjectBeingAddedHovering = nullptr;

    // Datenstrukturen, die wir zum Rendern brauchen ///////////////////////////////////////////////////////////////////

    /**
     * @brief Hilfsstruktur, die zum Zeichnen der Map-Objekte benötigt wird. Wir wollen sie nicht jedes Frame neu
     * anlegen, weil sie immer so groß wie die Karte ist.
     * @sa renderMap()
     */
    RectangleData<char>* mapObjectAlreadyDrawnThere = nullptr;

    /**
     * @brief Datenstruktur, mit der uns merken, was temporär zu zeichnen ist. Das sind alle Objekte, die gerade
     * positioniert werden oder im Begriff sind, abgerissen zu werden.
     */
    RectangleData<MapTileTemporialy*>* mapTilesToDrawTemporarily = nullptr;

    /**
     * @brief Zeiger auf ein Gebäude (durch `mapObjectsBeingAdded` verwaltet), für welches der
     * Einzugsbereich gezeichnet werden soll.
     */
    Building* buildingToDrawCatchmentArea = nullptr;

#ifdef DEBUG
    /**
     * @brief Hilfsgrafik mit Gitternetz-Linien-Overlay
     */
    SDLGraphic* debugGridOverlayGraphic;
#endif

public:
    /**
     * @brief Konstruktor. Positioniert die Komponente entsprechend `Consts::mapClipRect`.
     * @param context Dependency
     */
    GuiMap(const Context* const context);

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
     * @brief Callback, wenn ein neues Spiel startet. In diesem Fall müssen wir `mapObjectAlreadyDrawnThere`
     * auf die neue Map-Größe bringen.
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
    /**
     * @brief Zeichnet den Einzugsbereich eines Gebäudes.
     * @param renderer (Dependency)
     * @param building Gebäude
     */
    void drawCatchmentArea(IRenderer* const renderer, const Building* building);

    /**
     * @brief interner Klickhandler, wenn in die Karte geklickt wurde.
     * @param mouseX X-Fenster-Koordinate, die geklickt wurde
     * @param mouseY Y-Fenster-Koordinate, die geklickt wurde
     */
    void onClickInMap(int mouseX, int mouseY);

    /**
     * Prüft, ob eine bestimmte Struktur an eine bestimmte Position gesetzt werden darf.
     *
     * @param mapCoords Map-Koordinate, wo die Struktur gesetzt werden soll
     * @param structureType Typ der zu setzenden Struktur
     * @param view Ausrichtung der Struktur
     * @sa PLACING_STRUCTURE-Konstanten
     * @return Bitmaske, die angibt, ob das Gebäude gesetzt werden darf.
     */
    unsigned char isAllowedToPlaceStructure(
        const MapCoords& mapCoords, StructureType structureType, const FourthDirection& view) const;

//    /**
//     * @brief Berechnet welcher konkrete StructureType für eine Straßenkachel verwendet werden muss. Als Eingabeparameter
//     * abstractStreetStructureType wird festgelegt, ob Pflasterstraße oder Feldweg und an welchen Map-Koordinaten die
//     * Grafik gesetzt werden soll. Diese Methode bestimmt ausgehend von umliegenden Wegen, ob Gerade, Kurve, T-Stück
//     * oder Kreuzung verwendet werden muss und gibt diesen StructureType zurück.
//     * @param mapCoords Map-Koordinaten, für die berechnet werden soll
//     * @param abstractStreetStructureType spezifiziert, welcher Typ von Weg gewünscht wird
//     * @return konkreter StructureType, der an dieser Stelle verwendet werden muss
//     * TODO Feldweg
//     */
//    StructureType getConcreteStreetStructureType(
//        const MapCoords& mapCoords, StructureType abstractStreetStructureType) const;

    /**
     * @brief Entfernt alle Objekte in `mapObjectsBeingAdded` und `mapTilesToDrawTemporarily` und setzt
     * `buildingToDrawCatchmentArea` auf `nullptr`.
     */
    void clearAllTemporarily();

    /**
    * @brief Beendet das Setzen neuer Gebäude. Alle Gebäude werden nun gesetzt.
    */
    void finishAddingStructure();

    /**
     * @brief Fügt ein Gebäude zu `mapObjectsBeingAdded` hinzu.
     * @param mapCoords Map-Koordinaten, wo das Gebäude platziert werden soll
     */
    void addCurrentStructureToMapObjectsBeingAdded(const MapCoords& mapCoords);

    /**
     * @brief Aktualisiert die `drawingFlags` der `mapObjectsBeingAdded` und `mapObjectBeingAddedHovering`.
     * Dies ist immer vor jedem Frame-Rendern notwendig, da sich ständig die verfügbaren Resourcen ändern.
     */
    void updateMapObjectsTemporarilyDrawingFlags();
};

#endif
