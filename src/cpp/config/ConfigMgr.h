#ifndef _CONFIG_MGR_H
#define _CONFIG_MGR_H

#include <unordered_map>
#include "defines.h"
#include "game/GoodsSlot.h"
#include "map/Building.h"
#include "map/Direction.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "utils/RectangleData.h"


enum MapObjectType : unsigned char;


/**
 * @brief Baukosten eines Gebäudes
 */
typedef
struct BuildingCosts {
    
    /**
     * @brief Münzen
     */
    unsigned int coins;
    
    /**
     * @brief Werkzeuge
     */
    unsigned int tools;
    
    /**
     * @brief Holz
     */
    unsigned int wood;
    
    /**
     * @brief Ziegel
     */
    unsigned int bricks;

public:
    BuildingCosts() : coins(0), tools(0), wood(0), bricks(0) {
    }

    BuildingCosts(unsigned int coins, unsigned int tools, unsigned int wood, unsigned int bricks)
        : coins(coins), tools(tools), wood(wood), bricks(bricks) {
    }

    /**
     * @brief Skalar-Multiplikations-Operator, der die Baukosten für mehrere Gebäude berechnet.
     * @param n Multiplikator
     * @return Baukosten für `n` Gebäude
     */
    BuildingCosts operator*(int n) const {
        return BuildingCosts(coins * n, tools * n, wood * n, bricks * n);
    }

} BuildingCosts;


/**
 * @brief gibt an, wie Strukturen platziert wird
 */
enum StructurePlacing {

    /**
     * @brief mehreren Struktur werden einzeln platziert
     */
    INDIVIDUALLY,

    /**
     * @brief mehreren Struktur werden als Rechteck platziert
     */
    RECTANGLE,

    /**
     * @brief mehreren Struktur werden mittels eines Pfads platziert
     */
    PATH

};


typedef
struct BuildingConfig {
    /**
     * @brief Name des Gebäudes
     */
    const char* name;

    /**
     * @brief Bezeichnung in einem gespeicherten Spielstand
     */
    std::string nameInSavefile;

    /**
      * @brief gibt an, wie das Gebäude platziert wird
      */
    StructurePlacing structurePlacing;
    
    /**
     * @brief Einzugsbereich des Gebäudes. Es gilt hierbei 0 = außerhalb, 1 = Einzugsbereich.
     * 
     * Um das Rechteck korrekt anzuwenden, muss dieses symmetrisch um die Gebäudemitte angewandt werden.
     * Bsp: Ein 2x2-Gebäude mit einem 8x4-Einzugsbereich.
     *
     * <pre>
     * Richtig:                            Falsch:
     * -------------------------------     -------------------------------
     * | | | | | | | | | | | | | | | |     | | | | | | | | | | | | | | | |
     * ------=================--------     -------------------------------
     * | | | I | | | | | | | I | | | |     | | | | | | | | | | | | | | | |
     * ------I---------------I--------     --------=================------
     * | | | I | | |X|X| | | I | | | |     | | | | I | |X|X| | | | I | | |
     * ------I---------------I--------     --------I---------------I------
     * | | | I | | |X|X| | | I | | | |     | | | | I | |X|X| | | | I | | |
     * ------I---------------I--------     --------I---------------I------
     * | | | I | | | | | | | I | | | |     | | | | I | | | | | | | I | | |
     * ------=================--------     --------I---------------I------
     * | | | | | | | | | | | | | | | |     | | | | I | | | | | | | I | | |
     * -------------------------------     --------=================------
     * </pre>
     *
     * Der Einzugsbereich muss desweiteren folgende Bedingungen erfüllen, damit die Anwendung korrekt funktioniert:
     * - Es darf keine Leerzeile/-spalte geben. Das Rechteck muss so klein wie möglich gewählt sein.
     * - Der Einzugsbereich darf nicht nach innen gewölbt sein. Nur runde oder rechteckige Formen sind erlaubt.
     */
    RectangleData<char>* catchmentArea = nullptr;
    
    /**
     * @brief Baukosten
     */
    BuildingCosts buildingCosts;

    /**
     * @brief Güter, die verbraucht und hergestellt werden.
     */
    ProductionSlots buildingProduction;

    /**
     * @brief gibt an, wie viele Güter pro 60 Sekunden generiert werden
     */
    double productionRate = 0;

    /**
     * @brief gibt an, wie viele Güter vom ersten Input-Slot pro 60 Sekunden verbraucht werden
     */
    double inputConsumptionRate = 0;

    /**
     * @brief gibt an, wie viele Güter vom zweiten Input-Slot pro 60 Sekunden verbraucht werden
     */
    double input2ConsumptionRate = 0;

    /**
     * @brief gibt für Nicht-Wohngebäude an, wie viele Einwohner sich im Gebäude befinden. Jede Plantage,
     * jeder Produktionsbetrieb, wie auch öffentliche Gebäude haben diese fixe Einwohnerzahl pro Gebäude.
     */
    unsigned char inhabitants = 0;

    
    ~BuildingConfig() {
        if (catchmentArea != nullptr) {
            delete catchmentArea;
        }
    }

    /**
     * @return Gebäudenamen
     */
    const char* getName() const {
        return name;
    }
    
    /**
     * @return Einzugsbereich
     */
    RectangleData<char>* getCatchmentArea() const {
        return catchmentArea;
    }
    
    /**
     * @return Baukosten
     */
    const BuildingCosts* getBuildingCosts() const {
        return &buildingCosts;
    }

    /**
     * @return produzierte und verbrauchte Güter
     */
    const ProductionSlots* getBuildingProduction() const {
        return &buildingProduction;
    }

} BuildingConfig;


/**
 * @page tiles Kacheln
 *
 * Generierung
 * ===========
 *
 * Die Landschaftskacheln (tiles) werden über die Datei `data/tiles.xml` gesteuert.
 * In der Blender-Datei sind die Kachel-Objekte speziell benannt `tile-{tilename}`. Das Python-Script, was das Rendering
 * übernimmt erzeugt für jede Kachel dann 4 Grafiken `{tilename}-angle{0|90|180|270}.png`. Mittels des PHP-Scripts
 * und der `tiles.xml` wird am Ende eine einzige Grafik `tileset.png` generiert, die im Spiel verwendet wird.
 *
 * Tiled, Views und der View-Offset
 * ================================
 *
 * Tiled benutzt `tileset.png` als Tileset. Hier gibt es nun ein Problem, weil Tiled keine Ansichten unterstützt.
 * Die Karte in Tiled ist immer in Süd-Ansicht zu sehen. Im Tileset sind aber Grafiken aus allen Ansichten vertreten.
 *
 * Wir benutzen diese Grafiken auch alle, um zu sparen. Z.&nbsp;B. kann die Grafik von der West-Küste (in Tiled aus
 * Süden betrachtet) für die Ost-Küste (im Spiel später aus Norden betrachtet) wiederverwendet werden. Das Problem
 * besteht nun darin, dass die MapTileConfig nur Informationen für *eine* Kachel (= dem Verbund aller 4 Ansichten)
 * enthält und wir beim Zeichnen nicht mehr wissen, welche Grafik wir eigentlich brauchen.
 *
 * Nochmal verdeutlicht: In Tiled gibt es 4 Kacheln, während im Spiel nur 1 MapTileConfig für diese da ist.
 *
 * Deshalb gibt es den `View-Offset`, den der ConfigMgr uns pro Tiled-Kachel-ID liefern kann. Dieser Offset ist eine
 * Zahl zwischen 0 und 3, die wir auf die View addieren müssen, um die richtige View für die Grafik zu erhalten.
 *
 * Blender generiert 4 Ansichten, die entsprechend mit `angle` 0, 90, 180 und 270  benannt sind. Und in der `tiles.xml`
 * sind die Views entsprechend `south`, `east`, `north` und `west` konfiguriert.
 * **Wichtig:** Da die Grafiken mehrfach benutzt werden, sind die Angaben keine Infos, aus welcher Ansicht die Landschaft
 * gezeigt wird. Die Ansichten geben lediglich den Zusammenhang zwischen den 4 Grafiken an, also z.&nbsp;B. dass die
 * `south`-Grafik die Landschaft von der gegenüberliegenden Seite, wie die `north`-Grafik zeigt. Die Bedeutung, wann
 * welche Grafik benutzt werden muss, wird erst in Kombination mit dem View-Offset zugemessen.
 */


/**
 * @brief Konfiguration für eine Kachel
 */
typedef
struct MapTileConfig {

    /**
     * @brief Name der Kachel. Es gibt mehrere Kacheln, die denselben Namen haben. Diese unterscheiden sich in der view.
     * Wir ordnen damit die Kacheln untereinander zu, was zu rendern ist, wenn die Karte gedreht wird.
     */
    std::string tileName;

    /**
     * @brief Map, die für jede der 4 Ansichten die Pixeloffsets im Tileset angibt, wo die Grafik für diese View zu
     * finden ist. Das Paar enthalt X-Koordinate als ersten Member, Y-Koordinate als zweiten.
     */
    std::unordered_map<FourthDirection, std::pair<int, int>> mapTileViewsOffsetXYInTileset;

    /**
     * @brief Flag, ob diese Kachel grundsätzlich für das Drauflaufen (Routing-Algorithmus) und
     * Bebauen (Setzen neuer Gebäude/Strukturen) geeignet ist.
     */
    bool isWalkableAndBuildable;

    /**
     * @brief Flag, ob die Kachel Ozean ist. Wird für die Pixel auf der Minimap benutzt.
     */
    bool isOcean;

} MapTileConfig;


/**
 * @brief Klasse, die die Konfiguration enthält
 */
class ConfigMgr {
    
private:
    /**
     * @brief Array mit Zeigern auf die Gebäude-Konfigurationen
     */
    BuildingConfig** buildingConfigs;

    /**
     * @brief Map, die eine Zuordnung von `nameInSavefile` auf den zugehörigen `MapObjectType` macht.
     * Diese Map brauchen wir beim Laden eines Spielstands.
     */
    std::unordered_map<std::string, MapObjectType> mapMapObjectNameInSavefile;

    /**
     * @brief Map, die Tilenamen auf die Tile-Konfiguration abbildet
     */
    std::unordered_map<std::string, MapTileConfig> mapTileConfigs;

    /**
     * @brief Map, die eine Tiled-Kachel-ID auf die Tile-Konfiguration abbildet.
     * Siehe hierzu auch @ref tiles.
     */
    std::unordered_map<int, const MapTileConfig*> tiledIdToMapTileConfig;

    /**
     * @brief Map, die eine Tiled-Kachel-ID auf den zugehörigen View-Offset abbildet.
     * Siehe @ref tiles zur Erklärung des View-Offsets..
     */
    std::unordered_map<int, unsigned char> tiledIdToViewOffset;

    /**
     * @brief Referenz auf die Tile-Konfiguration des Ozeans
     */
    MapTileConfig* mapTileConfigOcean;

public:
    // TODO alles aus Datei einlesen
    /**
     * @brief Konstruiert die Konfiguration
     */
    ConfigMgr();

    /**
     * Gibt den belegten Speicher der Konfigurationen wieder frei
     */
    VIRTUAL_ONLY_IN_TESTS
    ~ConfigMgr();
    
    /**
     * @brief Liefert die Konfiguration eines Map-Objekts zurück
     * @param mapObjectType Typ des Map-Objekts
     * @return Konfiguration
     */
    VIRTUAL_ONLY_IN_TESTS
    const BuildingConfig* getBuildingConfig(MapObjectType mapObjectType) const {
        return buildingConfigs[mapObjectType];
    }

    /**
     * @brief Liefert den `MapObjectType` einer Struktur/Gebäudes ausgehend von einem `nameInSavefile` zurück.
     * @param nameInSavefile Name des Map-Objekts im Spielstand
     * @return `MapObjectType`. `NO_MAP_OBJECT`, wenn der Name nicht gefunden wurde
     */
    const MapObjectType getMapObjectType(const std::string& nameInSavefile) const {
        auto iter = mapMapObjectNameInSavefile.find(nameInSavefile);
        if (iter != mapMapObjectNameInSavefile.end()) {
            return iter->second;
        } else {
            return MapObjectType::NO_MAP_OBJECT;
        }
    }

    /**
     * @brief Liefert eine Referenz auf die Map mit den Tile-Konfigurationen zurück.
     * @return Map mit den Tile-Konfigurationen
     */
    const std::unordered_map<std::string, MapTileConfig>& getMapTileConfigs() const {
        return mapTileConfigs;
    }

    /**
     * @brief Liefert einen Zeiger auf die Tile-Konfiguration zurück, die zu einer bestimmten Tile-Kachel-ID gehört.
     * @param tileIndex Kachel-ID aus Tiled
     * @return Tile-Konfiguration
     */
    const MapTileConfig* getMapTileConfigByTiledId(unsigned char tileIndex) const {
        return tiledIdToMapTileConfig.at((int) tileIndex);
    }

    /**
     * @brief Liefert den [View-Offset](@ref tiles), der zu einer bestimmten Tile-Kachel-ID gehört.
     * @param tileIndex Kachel-ID aus Tiled
     * @return View-Offset
     */
    unsigned char getViewOffsetByTiledId(unsigned char tileIndex) const {
        return tiledIdToViewOffset.at((int) tileIndex);
    }

    /**
     * @brief Liefert die Tile-Konfiguration für den Ozean zurück.
     * @return Tile-Konfiguration
     */
    const MapTileConfig* getMapTileConfigOcean() const {
        return mapTileConfigOcean;
    }

private:
    /**
     * @brief Konstruiert (TODO aus einer Datei laden) die Konfiguration der Gebäude
     */
    void loadBuildingConfig();

    /**
     * @brief Lädt die Konfiguration der Gelände-Kacheln
     */
    void loadTilesConfig();

    /**
     * @brief Helper, der aus einem XML-Attribut einen Bool-Wert liest. Der Wert muss entweder "true" oder "false"
     * sein. Ist das Attribut nicht vorhanden, wird ein bestimmter Default-Wert benutzt.
     *
     * @param attribute XML-Attribut (kann `nullptr` sein)
     * @param defaultValue Default-Wert, der verwendet wird, wenn das Attribut nicht vorhanden ist.
     */
    bool xmlAttributeToBool(rapidxml::xml_attribute<>* attribute, bool defaultValue);
};

#endif
