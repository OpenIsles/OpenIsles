#ifndef _CONFIG_MGR_H
#define _CONFIG_MGR_H

#include <list>
#include <gtest/gtest.h>
#include <unordered_map>
#include "defines.h"
#include "config/Good.h"
#include "game/GoodsSlot.h"
#include "map/Building.h"
#include "map/Direction.h"
#include "map/MapObjectType.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"


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

    FRIEND_TEST(ConfigMgrTest, parseCatchmentArea);
    
private:
    /**
     * @brief Map aller Güter. Map-Key ist der Name des Guts.
     * In dieser Datenstruktur sind die einzigen Instanzen von `Good`. Alle anderen Objekte referenzieren diese
     * per const-Zeiger.
     */
    std::unordered_map<std::string, Good> goodsMap;

    /**
     * @brief Liste aller Güter. Im Gegensatz zur Map haben wir mit dieser Liste eine Sortierung der Güter
     */
    std::list<const Good*> goodsList;

    /**
     * @brief Map aller Map-Objekt-Typen. Map-Key ist der Name des Map-Objekt-Typs.
     */
    std::unordered_map<std::string, MapObjectType> mapObjectTypesMap;

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
     * @brief Liefert alle verfügbaren Güter im Spiel als Map zurück.
     * @return Map mit allen Gütern
     */
    const std::unordered_map<std::string, Good>& getAllGoods() const {
        return goodsMap;
    }

    /**
     * @brief Liefert alle verfügbaren Güter im Spiel als sortierte Liste zurück.
     * @return Liste mit allen Gütern
     */
    const std::list<const Good*>& getAllGoodsOrdered() const {
        return goodsList;
    }

    /**
     * @brief Liefert ein Gut zurück
     * @param goodName Name des Guts
     * @return Gut
     */
    const Good* getGood(const std::string& goodName) const {
        return &goodsMap.at(goodName);
    }

    /**
     * @brief Liefert den `MapObjectType` einer Struktur/Gebäudes ausgehend von seinem Namen zurück.
     * @param name Name des Map-Objekts
     * @return `MapObjectType`. `nullptr`, wenn es keinen solchen Map-Objekt-Typ gibt.
     */
    const MapObjectType* getMapObjectType(const std::string& name) const {
        auto iter = mapObjectTypesMap.find(name);
        if (iter != mapObjectTypesMap.end()) {
            return &iter->second;
        } else {
            return nullptr;
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
     * @brief Lädt die Waren
     */
    void loadGoods();

    /**
     * @brief Lädt die Konfiguration der verfügbaren Map-Objekt-Typen
     */
    void loadMapObjectTypes();

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
    static bool xmlAttributeToBool(rapidxml::xml_attribute<>* attribute, bool defaultValue);

    /**
     * @brief Helper, der aus dem Wert eines `<catchment-area>`-Tags die Datenstruktur des Einzugsbereichs liest.
     * Er ermittelt Breite/Höhe des Einzugsbereichs und stellt sicher, dass der String ordentlich geparst werden kann.
     *
     * @param catchmentAreaValue String, der den Wert des XML-Tags `<catchment-area>` enthält
     * @return Zeiger auf die `RectangleData`-Struktur mit dem Einzugsbereich
     */
    static RectangleData<char>* parseCatchmentArea(const char* catchmentAreaValue);

    /**
     * @brief Liest aus einem `<production-slots>`-Unterknoten (`<output>`, `<input>` oder `<input2>`) die Daten
     * und füllt damit den entsprechenden `GoodsSlot`.
     *
     * @param goodSlot Datenstruktur, die gefüllt wird
     * @param produtionSlotNode Zeiger auf den XML-Knoten. Darf nicht `nullptr` sein.
     */
    void readGoodSlotConfig(GoodsSlot& goodSlot, rapidxml::xml_node<>* produtionSlotNode);
};

#endif
