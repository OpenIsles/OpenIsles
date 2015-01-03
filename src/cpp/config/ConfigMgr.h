#ifndef _CONFIG_MGR_H
#define _CONFIG_MGR_H

#include "game/GoodsSlot.h"
#include "map/Building.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "utils/RectangleData.h"


enum StructureType : unsigned char;


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
    
} BuildingCosts;


typedef
struct BuildingConfig {
    /**
     * @brief Name des Gebäudes
     */
    const char* name;
    
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
 * @brief Infos über eine bestimmte Geländekachel
 */
typedef
struct MapTileConfig {

    /**
     * Dateiname der Grafik
     */
    const char* graphicsFile;

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
     * Array mit Zeigern auf die Gebäude-Konfigurationen
     */
    BuildingConfig** buildingConfigs;

    /**
     * Array mit Zeigern auf die Gebäude-Konfigurationen
     */
    MapTileConfig** mapTileConfigs;

public:
    // TODO alles aus Datei einlesen
    /**
     * @brief Konstruiert die Konfiguration
     */
    ConfigMgr();

    /**
     * Gibt den belegten Speicher der Konfigurationen wieder frei
     */
    ~ConfigMgr();
    
    /**
     * @brief Liefert die Konfiguration eines Gebäudes zurück
     * @param structureType Typ des Gebäudes
     * @return Konfiguration
     */
    const BuildingConfig* const getBuildingConfig(StructureType structureType) const {
        return buildingConfigs[structureType];
    }

    /**
     * @brief Liefert die Konfiguration einer Gelände-Kachel zurück
     * @param tileIndex Index der Kachel
     * @return Konfiguration
     */
    const MapTileConfig* const getMapTileConfig(unsigned char tileIndex) const {
        return mapTileConfigs[tileIndex];
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
