#ifndef _MAP_OBJECT_TYPE_H
#define _MAP_OBJECT_TYPE_H

#include <memory>
#include <string>
#include <unordered_map>
#include "config/BuildingCosts.h"
#include "config/MapTileType.h"
#include "game/PopulationTier.h"
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
    BUILDING,

    /**
     * @brief `Carrier`
     */
    CARRIER
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
     * @brief Betriebskosten
     */
    struct {
        unsigned char running = 0;        ///< laufender Betrieb
        unsigned char decommissioned = 0; ///< stillgelegt
    } operatingCosts;

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
     * @brief (nur für Häuser) zugeordnete Bevölkerungsgruppe
     */
    const PopulationTier* populationTier;

    /**
     * @brief gibt an, wie viele Träger das Gebäude maximal haben kann.
     *
     * Je nach Gebäude sind die Träger verschieden, z.&nbsp;B. hat
     * - ein Fischer ein Fischerboot (="Produktionsträger"),
     * - eine Schaffarm bis zu drei grasende Schafe (="Produktionsträger"),
     * - eine Tabakplantage eine Erntnerin (="Produktionsträger"),
     * - der Marktplatz bis zu zwei Marktkarren (=Warenträger),
     * - eine Erzschmelze einen Träger (=Warenträger, der Erz und Holz holt),
     * - eine Feuerwehr einen Feuerwehrmann (="Arbeits-Map-Objekt")
     * als Träger.
     */
    unsigned char maxCarriers = 0;

    // TODO vermutlich sinnvoller, wenn Carrier einen eigenen CarrierConfig-Datentyp haben
    struct {
        /**
         * @brief (nur für `Building`, die Waren holen) Typ des Trägers, den dieses Gebäude losschickt
         */
        const MapObjectType* mapObjectType = nullptr;

        /**
         * @brief (nur für `Carrier`) gibt an, wie viel der Träger maximal tragen kann.
         */
        unsigned char capacity = 0;

        /**
         * @brief (nur für `Carrier`) ordnet Animations-Zustände einem GraphicSet-Keys zu
         */
        std::unordered_map<std::string, std::string> animations;

    } carrier;

    /**
     * @brief "Sekunden für 1t Produktion"
     * Für Träger: Zeit, die der Träger braucht, um 1t zu ernten
     * Für Produktionsgebäude: Zeit, die das Gebäude braucht, um 1t Output-Gut herzustellen
     */
    double secondsToProduce = 0;

    /**
     * @brief (nur für Gebäude) Anzahl Güter aus dem input-Slot, die verbraucht werden, um 1t zu produzieren
     */
    // TODO später für Goldschmiede explizit überprüfen. Das is das einzige Gebäude, was keine Ganzzahl hat
    double inputAmountForProduction = 0;

    /**
     * @brief (nur für Gebäude) Anzahl Güter aus dem input2-Slot, die verbraucht werden, um 1t zu produzieren
     */
    double input2AmountForProduction = 0;

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
