#include "graphics/graphic/IGraphic.h"
#include "map/coords/MapCoords.h"

#ifndef _MAP_COORD_UTILS_H
#define _MAP_COORD_UTILS_H

class Building;
class Map;
class Rect;

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
	 * @param mapCoords Map-Koordinaten (Eingabe)
	 * @param screenX Screen-X-Koordinate (Ausgabe)
	 * @param screenY Screen-Y-Koordinate (Ausgabe)
	 */
	static void mapToScreenCoordsCenter(const MapCoords& mapCoords, int& screenX, int& screenY);

    /**
	 * @brief Rechnet Screen- in Map-Koordinaten um.
	 * @param screenX Screen-X-Koordinate
	 * @param screenY Screen-Y-Koordinate
	 * @return Map-Koordinaten
	 */
	static MapCoords screenToMapCoords(int screenX, int screenY);

    /**
     * @brief Berechnet ausgehend von Map-Koordinaten und einer Grafik das SDL-Rechteck, an welche Stelle
     * die Grafik gesetzt werden muss.
     *
     * @param map (Dependency)
     * @param mapX Map-X-Koordinate (Eingabe)
	 * @param mapY Map-Y-Koordinate (Eingabe)
     * @param elevation Elevation, auf die die Grafik gezeichnet werden soll (Eingabe)
     * @param graphic zu zeichnende MapObject-Grafik (Eingabe)
     * @param rect Rechteck mit den Draw-Koordinaten, wo die Grafik hingezeichnet werden muss (Ausgabe)
     */
    static void mapToDrawCoords(
        Map* map, int mapX, int mapY, int elevation, const IGraphic* graphic, Rect* rect);

    /**
     * @brief Berechnet ausgehend von Map-Koordinaten und einer Grafik das SDL-Rechteck, an welche Stelle
     * die Grafik gesetzt werden muss.
     * Diese Methode ist von der Funktion analog mapToDrawCoords(int, int, int, IGraphic*, Rect*),
     * rechnet aber mit double-Eingabewerten.
     *
     * @param map (Dependency)
     * @param mapX Map-X-Koordinate (Eingabe)
     * @param mapY Map-Y-Koordinate (Eingabe)
     * @param elevation Elevation, auf die die Grafik gezeichnet werden soll (Eingabe)
     * @param graphic zu zeichnende MapObject-Grafik (Eingabe)
     * @param rect Rechteck mit den Draw-Koordinaten, wo die Grafik hingezeichnet werden muss (Ausgabe)
     */
    static void mapToDrawCoords(
        Map* map, double mapX, double mapY, int elevation, const IGraphic* graphic, Rect* rect);

    /**
     * @brief Berechnet für ein bestimmtes Gebäude das SDL-Rechteck, an welche Stelle die Grafik gesetzt werden muss.
     * @param map (Dependency)
     * @param graphicsMgr (Dependency)
     * @param building Gebäude (Eingabe)
     * @param rect Rechteck mit den Draw-Koordinaten, wo die Grafik hingezeichnet werden muss (Ausgabe)
     */
    static void getDrawCoordsForBuilding(Map* map, IGraphicsMgr* graphicsMgr, Building* building, Rect* rect);

    /**
     * @brief Ermittelt für die aktuelle Position des Mauszeigers, welche Map-Koordinate dort liegt.
     * Es wird immer die elevatete Position genommen, selbst, wenn der Mauszeiger auf dem Wasser steht.
     *
     * @param map (Dependency)
     * @param mouseCurrentX X-Koordinate des Mauszeigers
     * @param mouseCurrentY Y-Koordinate des Mauszeigers
     * @return Map-Koordinaten
     */
    static MapCoords getMapCoordsUnderMouse(Map* map, int mouseCurrentX, int mouseCurrentY);

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
     * @param rect Rechteck mit den Draw-Koordinaten, wo die Grafik hingezeichnet werden muss (Ausgabe)
     */
    static void screenToDrawCoords(
        Map* map, int screenX, int screenY, int elevation, const IGraphic* graphic, Rect* rect);

};

#endif

