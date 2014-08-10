#ifndef _BUILDING_CONFIG_MGR_H
#define _BUILDING_CONFIG_MGR_H

#include "utils/RectangleData.h"


enum StructureType : unsigned char;

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
     * 
     * Der Einzugsbereich muss desweiteren folgende Bedingungen erfüllen, damit die Anwendung korrekt funktioniert:
     * - Es darf keine Leerzeile/-spalte geben. Das Rechteck muss so klein wie möglich gewählt sein.
     * - Der Einzugsbereich darf nicht nach innen gewölbt sein. Nur runde oder rechteckige Formen sind erlaubt.
     */
    RectangleData<char>* catchmentArea = nullptr;   
    
    ~BuildingConfig() {
        if (catchmentArea != nullptr) {
            delete catchmentArea;
        }
    }

    /**
     * @return Gebäudenamen
     */
    const char* GetName() const {
        return name;
    }
    
    /**
     * @return Einzugsbereich
     */
    RectangleData<char>* getCatchmentArea() const {
        return catchmentArea;
    }
    
} BuildingConfig;


/**
 * @brief Klasse, die die Konfiguration der Gebäude enthält
 */
class BuildingConfigMgr {
    
private:
    BuildingConfig** configs;  

public:
    // TODO aus Datei einlesen
    /**
     * @brief Konstruiert die Konfiguration
     */
    BuildingConfigMgr();
    ~BuildingConfigMgr();
    
    /**
     * @brief Liefert die Konfiguration eines Gebäudes zurück
     * @param structureType Typ des Gebäudes
     */
    const BuildingConfig* const getConfig(StructureType structureType) {
        return configs[structureType];
    }
};

#endif
