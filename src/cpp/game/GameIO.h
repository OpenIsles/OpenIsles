#ifndef _GAME_IO_H
#define _GAME_IO_H

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

/**
 * @brief Klasse mit Methoden zum Laden und Speichern (TODO) des Spielstands
 */
class GameIO {

public:
    /**
     * @brief Lädt einen Spielstand von einer tiled-TMX-Datei
     * @param filename Dateiname mit den zu ladenden Spielstand-Daten
     */
    static void loadGameFromTMX(const char* filename);

private:
    /**
     * @brief Lädt die Spieler aus der tiled-TMX-Datei
     * @param mapNode <map>-Knoten aus der tiled-Datei
     */
    static void loadPlayers(rapidxml::xml_node<>* mapNode);

    /**
     * @brief Lädt die Karte und Inseln aus der tiled-TMX-Datei
     * @param objectgroupIslesNode <objectgroup>-Knoten der "isles"-Ebene aus der tiled-Datei
     */
    static void loadMap(rapidxml::xml_node<>* objectgroupIslesNode);

    /**
     * @brief Lädt die Siedlungen aus der tiled-TMX-Datei
     * @param objectgroupColoniesNode <objectgroup>-Knoten der "colonies"-Ebene aus der tiled-Datei
     */
    static void loadColonies(rapidxml::xml_node<>* objectgroupColoniesNode);

    /**
     * @brief Lädt die Strukturen und Gebäude aus der tiled-TMX-Datei
     * @param objectgroupStructuresNode <objectgroup>-Knoten der "structures"-Ebene aus der tiled-Datei
     */
    static void loadStructures(rapidxml::xml_node<>* objectgroupStructuresNode);


    /**
     * @brief Helper, um aus einem <object>-Knoten die Map-Koordinaten zu ermitteln
     * @param objectNode <object>-Knoten aus einer XML-Datei (IN)
     * @param mapX X-Map-Koordinate (OUT)
     * @param mapY Y-Map-Koordinate (OUT)
     */
    static void getMapCoordsFromObjectNode(rapidxml::xml_node<>* objectNode, int& mapX, int& mapY);

    /**
     * @brief Helper, um aus einem Knoten eine bestimmten Property-Wert auszulesen. Es werden die <property>-Knoten
     * innerhalb des <properties>-Knoten durchsucht und der value-Attributwert zurückgegeben.
     * @param objectNode <object>-Knoten aus einer XML-Datei
     * @param propertyName Name des Property, die ermittelt werden soll
     * @return value-Attributwert der Property oder nullptr, wenn die Property nicht existiert.
     * Vorsicht: Der char-Zeiger gehört zur XML und verfällt, nachdem die XML zerstört ist. Braucht man die Zeichenkette,
     * muss sie abkopiert werden.
     */
    static const char* getPropertyValueFromPropertiesNode(rapidxml::xml_node<>* propertiesNode, const char* propertyName);

};

#endif

