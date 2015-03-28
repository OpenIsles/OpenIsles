#ifndef _GAME_IO_H
#define _GAME_IO_H

#include "config/ConfigMgr.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

/**
 * @brief Klasse mit Methoden zum Laden und Speichern (TODO) des Spielstands
 */
class GameIO {

public:
    /**
     * @brief Lädt einen Spielstand von einer tiled-TMX-Datei
     * @param game Spiel, in das der Spielstand geladen werden soll
     * @param configMgr (Dependency)
     * @param graphicsMgr (Dependency)
     * @param filename Dateiname mit den zu ladenden Spielstand-Daten
     */
    static void loadGameFromTMX(
        Game* game, const ConfigMgr* configMgr, const IGraphicsMgr* graphicsMgr, const char* filename);

private:
    /**
     * @brief Lädt die Spieler aus der tiled-TMX-Datei
     * @param game Spiel, in das der Spielstand geladen werden soll
     * @param mapNode <map>-Knoten aus der tiled-Datei
     */
    static void loadPlayers(Game* game, rapidxml::xml_node<>* mapNode);

    /**
     * @brief Lädt die Karte und Inseln aus der tiled-TMX-Datei
     * @param game Spiel, in das der Spielstand geladen werden soll
     * @param configMgr (Dependency)
     * @param graphicsMgr (Dependency)
     * @param objectgroupIslesNode <objectgroup>-Knoten der "isles"-Ebene aus der tiled-Datei
     */
    static void loadMap(
        Game* game, const ConfigMgr* configMgr, const IGraphicsMgr* graphicsMgr, rapidxml::xml_node<>* objectgroupIslesNode);

    /**
     * @brief Lädt die Siedlungen aus der tiled-TMX-Datei
     * @param game Spiel, in das der Spielstand geladen werden soll
     * @param objectgroupColoniesNode <objectgroup>-Knoten der "colonies"-Ebene aus der tiled-Datei
     */
    static void loadColonies(Game* game, rapidxml::xml_node<>* objectgroupColoniesNode);

    /**
     * @brief Lädt die Map-Objekte (Strukturen, Gebäude und Wälder) aus der tiled-TMX-Datei
     * @param game Spiel, in das der Spielstand geladen werden soll
     * @param configMgr (Dependency)
     * @param objectgroupMapObjectsNode <objectgroup>-Knoten der "mapobjects"-Ebene aus der tiled-Datei
     */
    static void loadMapObjects(Game* game, const ConfigMgr* configMgr, rapidxml::xml_node<>* objectgroupMapObjectsNode);

    /**
     * @brief Helper, um aus einem <object>-Knoten die Map-Koordinaten zu ermitteln
     * @param objectNode <object>-Knoten aus einer XML-Datei
     * @return Map-Koordinate
     */
    static MapCoords getMapCoordsFromObjectNode(rapidxml::xml_node<>* objectNode);

    /**
     * @brief Helper, um aus einem Knoten eine bestimmten Property-Wert auszulesen. Es werden die <property>-Knoten
     * innerhalb des <properties>-Knoten durchsucht und der value-Attributwert zurückgegeben.
     *
     * @param objectNode <object>-Knoten aus einer XML-Datei
     * @param propertyName Name des Property, die ermittelt werden soll
     * @return value-Attributwert der Property oder nullptr, wenn die Property nicht existiert.
     * Vorsicht: Der char-Zeiger gehört zur XML und verfällt, nachdem die XML zerstört ist. Braucht man die Zeichenkette,
     * muss sie abkopiert werden.
     */
    static const char* getPropertyValueFromPropertiesNode(rapidxml::xml_node<>* propertiesNode, const char* propertyName);

    /**
     * @brief Helper, um aus einem <properties>-Knoten die Eigenschaft "view" auszulesen und die Ansicht zurückzuliefern.
     * Ist keine entsprechende Property vorhanden, wird die Südansicht (als Default, der wohl hauptsächlich benutzt wird)
     * zurückgeliefert.
     * @param propertiesNode <properties>-Knoten aus einer XML-Datei
     * @return Ansicht
     */
    static const EighthDirection getViewPropertyValueFromPropertiesNode(rapidxml::xml_node<>* propertiesNode);

};

#endif

