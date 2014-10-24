#include "graphics/MapObjectGraphic.h"

#ifndef _MAP_UTILS_H
#define _MAP_UTILS_H

struct SDL_Rect;
class MapObjectGraphic;

/**
 * Klasse mit statischen Helperklassen
 */
class MapUtils {
    
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
     * @param mapX Map-X-Koordinate (Eingabe)
	 * @param mapY Map-Y-Koordinate (Eingabe)
     * @param graphic zu zeichnende MapObject-Grafik (Eingabe)
     * @param rect SDL-Rechteck mit der Screen-Koordinaten, wo die Grafik hingezeichnet werden muss (Ausgabe)
     */
    static void mapToDrawScreenCoords(int mapX, int mapY, MapObjectGraphic* graphic, SDL_Rect* rect);

    /**
     * @brief Berechnet ausgehend von Map-Koordinaten und einer Grafik das SDL-Rechteck, an welche Stelle
     * die Grafik gesetzt werden muss.
     * Diese Methode ist von der Funktion analog mapToDrawScreenCoords(int, int, MapObjectGraphic*, SDL_Rect*),
     * rechnet aber mit double-Eingabewerten.
     *
     * @param mapX Map-X-Koordinate (Eingabe)
     * @param mapY Map-Y-Koordinate (Eingabe)
     * @param graphic zu zeichnende MapObject-Grafik (Eingabe)
     * @param rect SDL-Rechteck mit der Screen-Koordinaten, wo die Grafik hingezeichnet werden muss (Ausgabe)
     */
    static void mapToDrawScreenCoords(double mapX, double mapY, MapObjectGraphic* graphic, SDL_Rect* rect);
    
};

#endif

