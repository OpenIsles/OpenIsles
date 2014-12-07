#include "graphics/MapObjectGraphic.h"

#ifndef _MAP_COORD_UTILS_H
#define _MAP_COORD_UTILS_H

struct SDL_Rect;
class Building;
class Map;
class MapObjectGraphic;

/**
 * @page coordsystem Koordinatensysteme
 *
 * Die Karte stellt sich wiefolgt dar:
 * <pre>
 *                 y=0
 *             --/  (0,0)   x=0
 *       y <--/      ---        \--
 *                --/   \--        \--> x
 *             --/         \--
 *          --/               \--(w-1,0)
 *    (0,h-1)  --\         /--
 *                --\   /--
 *                   ---
 *                (w-1,h-1)
 * </pre>
 *
 * Koordinatensysteme:
 * - Obenstehend sind die Kachel- oder Map-Koordinaten (mapCoords) dargestellt.
 * - Bildschirm-Koordinaten (screenCoords) sind durch ein Pixel-Koordinatensystem definiert. Hierbei wird die
 *   Kachel mit mapCoords (0, 0) auf screenCoords (0, 0) gelegt.
 * - Die Bildschirm-Koordinate einer Kachel bestimmt die Position des Pixels links-oben am Kachelrechteck.
 * - Beim Zoomen ändert sich nicht die Zuordnung von map- zu screenCoords. Beim Zeichnen wird verkleinert, d.h.
 *   mehr Inhalt dargestellt. Auf Zoomstufe Verkleinerung 4x z.B. liegt neben dem Pixel das screenCoords (0, 0)
 *   darstellt, die screenCoords (4, 0).
 * - Screen-Koordinaten sind immer auf den Boden bezogen. Kommt Elevation ins Spiel, müssen die y-Screen-Koordinate
 *   entsprechend angepasst werden.
 * - Wenn von Zeichen-Koordinaten (drawCoords) gesprochen wird, sind das Pixel-Koordinaten für ein bestimmtes
 *   Objekt, um es mit dem aktuellen Scrolling-Offset (Map::screenOffsetX und Map::screenOffsetY) und der
 *   aktuellen Zoom-Stufe (Map::screenZoom) zu zeichnen (= die obere linke Ecke). drawCoords berücksichtigen die
 *   Elevation. Derartige Koordinaten sind immer "fix und fertig", damit wir sofort zeichnen können.
 * - Fenster-Koordinaten sind Pixel-Koordinaten innerhalb des Anwendungsfensters. Sie entsprechen Zeichen-Koordinaten,
 *   nur ein anderer Begriff, der benutzt wird, wenn es nicht ums Zeichnen von etwas geht.
 *
 * Siehe hierzu auch @a doc/map-coords.xcf .
 */

/**
 * @brief Klasse mit statischen Helperklassen
 */
class MapCoordUtils {

public:
    /**
	 * @brief Rechnet Map- in Screen-Koordinaten um. Die Screen-Koordinaten sind die der oberen linken Ecke der Kachel.
	 * @param mapX Map-X-Koordinate (Eingabe)
	 * @param mapY Map-Y-Koordinate (Eingabe)
	 * @param screenX Screen-X-Koordinate (Ausgabe)
	 * @param screenY Screen-Y-Koordinate (Ausgabe)
	 */
	static void mapToScreenCoords(int mapX, int mapY, int& screenX, int& screenY);

    /**
     * @brief Rechnet Map- in Screen-Koordinaten um. Die Screen-Koordinaten sind die der oberen linken Ecke der Kachel.
     * Diese Methode ist von der Funktion analog mapToScreenCoords(int, int, int&, int&), rechnet aber mit double-
     * Eingabewerten. Die Ergebniskoordinaten sind Ganzzahlen.
     *
     * @param mapX Map-X-Koordinate (Eingabe)
     * @param mapY Map-Y-Koordinate (Eingabe)
     * @param screenX Screen-X-Koordinate (Ausgabe)
     * @param screenY Screen-Y-Koordinate (Ausgabe)
     */
    static void mapToScreenCoords(double mapX, double mapY, int& screenX, int& screenY);

    /**
	 * @brief Rechnet Map- in Screen-Koordinaten um. Die Screen-Koordinaten sind in der Mitte der Kachel.
	 * @param mapX Map-X-Koordinate (Eingabe)
	 * @param mapY Map-Y-Koordinate (Eingabe)
	 * @param screenX Screen-X-Koordinate (Ausgabe)
	 * @param screenY Screen-Y-Koordinate (Ausgabe)
	 */
	static void mapToScreenCoordsCenter(int screenX, int screenY, int& mapX, int& mapY);

    /**
	 * @brief Rechnet Screen- in Map-Koordinaten um.
	 * @param screenX Screen-X-Koordinate (Eingabe)
	 * @param screenY Screen-Y-Koordinate (Eingabe)
	 * @param mapX Map-X-Koordinate (Ausgabe)
	 * @param mapY Map-Y-Koordinate (Ausgabe)
	 */
	static void screenToMapCoords(int screenX, int screenY, int& mapX, int& mapY);

    /**
     * @brief Berechnet ausgehend von Map-Koordinaten und einer Grafik das SDL-Rechteck, an welche Stelle
     * die Grafik gesetzt werden muss.
     *
     * @param map (Dependency)
     * @param mapX Map-X-Koordinate (Eingabe)
	 * @param mapY Map-Y-Koordinate (Eingabe)
     * @param elevation Elevation, auf die die Grafik gezeichnet werden soll (Eingabe)
     * @param graphic zu zeichnende MapObject-Grafik (Eingabe)
     * @param rect SDL-Rechteck mit den Draw-Koordinaten, wo die Grafik hingezeichnet werden muss (Ausgabe)
     */
    static void mapToDrawCoords(
        Map* map, int mapX, int mapY, int elevation, MapObjectGraphic* graphic, SDL_Rect* rect);

    /**
     * @brief Berechnet ausgehend von Map-Koordinaten und einer Grafik das SDL-Rechteck, an welche Stelle
     * die Grafik gesetzt werden muss.
     * Diese Methode ist von der Funktion analog mapToDrawCoords(int, int, int, MapObjectGraphic*, SDL_Rect*),
     * rechnet aber mit double-Eingabewerten.
     *
     * @param map (Dependency)
     * @param mapX Map-X-Koordinate (Eingabe)
     * @param mapY Map-Y-Koordinate (Eingabe)
     * @param elevation Elevation, auf die die Grafik gezeichnet werden soll (Eingabe)
     * @param graphic zu zeichnende MapObject-Grafik (Eingabe)
     * @param rect SDL-Rechteck mit den Draw-Koordinaten, wo die Grafik hingezeichnet werden muss (Ausgabe)
     */
    static void mapToDrawCoords(
        Map* map, double mapX, double mapY, int elevation, MapObjectGraphic* graphic, SDL_Rect* rect);

    /**
     * @brief Berechnet für ein bestimmtes Gebäude das SDL-Rechteck, an welche Stelle die Grafik gesetzt werden muss.
     * @param map (Dependency)
     * @param graphicsMgr (Dependency)
     * @param building Gebäude (Eingabe)
     * @param rect SDL-Rechteck mit den Draw-Koordinaten, wo die Grafik hingezeichnet werden muss (Ausgabe)
     */
    static void getDrawCoordsForBuilding(Map* map, GraphicsMgr* graphicsMgr, Building* building, SDL_Rect* rect);

    /**
     * @brief Ermittelt für die aktuelle Position des Mauszeigers, welche Map-Koordinate dort liegt.
     * Es wird immer die elevatete Position genommen, selbst, wenn der Mauszeiger auf dem Wasser steht.
     *
     * @param map (Dependency)
     * @param mouseCurrentX X-Koordinate des Mauszeigers (Eingabe)
     * @param mouseCurrentY Y-Koordinate des Mauszeigers (Eingabe)
     * @param mouseCurrentMapX Map-X-Koordinate (Ausgabe)
     * @param mouseCurrentMapY Map-Y-Koordinate (Ausgabe)
     */
    static void getMapCoordsUnderMouse(
        Map* map, int mouseCurrentX, int mouseCurrentY, int& mouseCurrentMapX, int& mouseCurrentMapY);

private:
    /**
     * @brief Rechnet Screen-Koordinaten, wo ein Gebäude gezeichnet werden soll, in die entsprechenden
     * Draw-Koordinaten um.
     * Interne Helper-Methode von mapToDrawCoords().
     *
     * @param map (Dependency)
     * @param screenX Screen-X-Koordinate (Eingabe)
     * @param screenY Screen-Y-Koordinate (Eingabe)
     * @param elevation Elevation, auf die die Grafik gezeichnet werden soll (Eingabe)
     * @param graphic zu zeichnende MapObject-Grafik (Eingabe)
     * @param rect SDL-Rechteck mit den Draw-Koordinaten, wo die Grafik hingezeichnet werden muss (Ausgabe)
     */
    static void screenToDrawCoords(
        Map* map, int screenX, int screenY, int elevation, MapObjectGraphic* graphic, SDL_Rect* rect);

};

#endif

