#ifndef _GUI_MAP_H
#define _GUI_MAP_H

#include <SDL_render.h>
#include "Context.h"
#include "gui/components/GuiBase.h"
#include "map/Directions.h"
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
#define PLACING_STRUCTURE_NO_RESOURCES           (1 << 0)

/**
 * @brief Setzen der Struktur ist hier nicht erlaubt, weil was im Weg ist.
 */
#define PLACING_STRUCTURE_SOMETHING_IN_THE_WAY   (1 << 1)

/**
 * @brief Setzen der Struktur ist hier nicht erlaubt, weil das Gebiet nicht erschlossen ist.
 * Grafisch wird dies dargestellt, indem die Struktur rot gezeichnet wird
 */
#define PLACING_STRUCTURE_ROOM_NOT_UNLOCK        (1 << 2)

/**
 * @brief Setzen der Struktur ist nicht erlaubt, da wir mitten auf dem Ozean außerhalb einer Insel sind.
 * Grafisch wird dies dargestellt, indem gar nix gezeichnet wird.
 */
#define PLACING_STRUCTURE_OUTSIDE_OF_ISLE        (1 << 3)



/**
 * @brief GUI-Element, was die Karte darstellt. Das ist der eigentliche Spielbereich.
 */
class GuiMap : public GuiBase {

private:
    /**
     * @brief Hilfsstruktur, die zum Zeichnen der Map-Objekte benötigt wird. Wir wollen sie nicht jedes Frame neu
     * anlegen, weil sie immer so groß wie die Karte ist.
     * @sa renderMap()
     */
    RectangleData<char>* mapObjectAlreadyDrawnThere = nullptr;

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
     */
    virtual void onEventElement(SDL_Event& event);

    /**
     * @brief Callback, wenn ein neues Spiel startet. In diesem Fall müssen wir `mapObjectAlreadyDrawnThere`
     * auf die neue Map-Größe bringen.
     */
    void onNewGame();

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
     * @brief interner Klickhandler, wenn in die Karte geklickt wurde, während wir im "Gebäude positionieren"-Modus
     * sind.
     * @param mapCoords Map-Koordinate, wo geklickt wurde
     */
    void onClickInMapWhileAddingStructure(const MapCoords& mapCoords);

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
        const MapCoords& mapCoords, StructureType structureType, const FourDirectionsView& view) const;

    /**
     * @brief Berechnet welcher konkrete StructureType für eine Straßenkachel verwendet werden muss. Als Eingabeparameter
     * abstractStreetStructureType wird festgelegt, ob Pflasterstraße oder Feldweg und an welchen Map-Koordinaten die
     * Grafik gesetzt werden soll. Diese Methode bestimmt ausgehend von umliegenden Wegen, ob Gerade, Kurve, T-Stück
     * oder Kreuzung verwendet werden muss und gibt diesen StructureType zurück.
     * @param mapCoords Map-Koordinaten, für die berechnet werden soll
     * @param abstractStreetStructureType spezifiziert, welcher Typ von Weg gewünscht wird
     * @return konkreter StructureType, der an dieser Stelle verwendet werden muss
     * TODO Feldweg
     */
    StructureType getConcreteStreetStructureType(
        const MapCoords& mapCoords, StructureType abstractStreetStructureType) const;

};

#endif
