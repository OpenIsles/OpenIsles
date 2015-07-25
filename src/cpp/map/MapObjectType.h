#ifndef _MAP_OBJECT_TYPE_H
#define _MAP_OBJECT_TYPE_H

#include <memory>
#include <string>
#include "config/BuildingCosts.h"
#include "config/MapTileType.h"
#include "game/ProductionSlots.h"
#include "utils/RectangleData.h"


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

/**
 * @brief Enum, der den Map-Objekten die zugehörige Klasse im Code zuordnet
 */
enum MapObjectTypeClass {
    /**
     * @brief `Harvestable`
     */
    HARVESTABLE,

    /**
     * @brief `Structure`
     */
    STRUCTURE,

    /**
     * @brief `Street`
     */
    STREET,

    /**
     * @brief `Building`
     */
    BUILDING
};


/**
 * @brief Datenstruktur, die alle Infos zu einem verfügbaren Map-Objekt-Typ enthält.
 *
 * Mit Ausnahme des ConfigMgr, der diese Datenstruktur verwaltet, sollte diese Struktur nur als `const MapObjectType*`
 * benutzt werden.
 */
struct MapObjectType {

    /**
     * @brief zugehörige Klasse im Code für dieses Map-Objekt
     */
    MapObjectTypeClass type;

    /**
     * @brief interne Bezeichnung (z.&nbsp;B. in einem gespeicherten Spielstand)
     */
    std::string name;

    /**
     * @brief Name des Map-Objekts (für den Spieler)
     */
    std::string title;

    /**
     * @brief Breite (X-Richtung) des Map-Objekts in Map-Koordinaten
     */
    unsigned char mapWidth;

    /**
     * @brief Höhe (Y-Richtung) des Map-Objekts in Map-Koordinaten
     */
    unsigned char mapHeight;

    /**
      * @brief gibt an, wie das Map-Objekt platziert wird
      */
    StructurePlacing structurePlacing;

    /**
     * @brief Maske von MapTileType-Enums, auf welchen Typen von Kacheln das Objekt gebaut werden darf
     */
    MapTileTypeInt placeableOnMapTileTypeMask;

    /**
     * @brief Einzugsbereich des Map-Objekts (optional). Es gilt hierbei 0 = außerhalb, 1 = Einzugsbereich.
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
    std::unique_ptr<RectangleData<char>> catchmentArea;

    /**
     * @brief Baukosten
     */
    BuildingCosts buildingCosts;

    /**
     * @brief Güter, die verbraucht und hergestellt werden.
     */
    ProductionSlots buildingProduction;

    /**
     * @brief gibt für Nicht-Wohngebäude an, wie viele Einwohner sich im Gebäude befinden. Jede Plantage,
     * jeder Produktionsbetrieb, wie auch öffentliche Gebäude haben diese fixe Einwohnerzahl pro Gebäude.
     */
    unsigned char inhabitants = 0;

    /**
     * @brief (nur für `Harvestable`) gibt das maximale Alter an, das diese Landschaft erreichen kann.
     */
    unsigned char maxAge = 0;

    /**
     * @brief Bool-Flag, was angibt, ob das hier ein Wald ist.
     * Dieses Flag wird verwendet, um in der Statuszeile statt dem Namen des Objekts die Fruchtbarkeit der Insel
     * anzuzeigen.
     */
    bool isForest = false;

};

#endif
