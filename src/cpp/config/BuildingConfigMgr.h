#ifndef _BUILDING_CONFIG_MGR_H
#define _BUILDING_CONFIG_MGR_H

typedef
struct BuildingConfig {
    /**
     * @brief Index des Gebäudes. Aktuell werden alle Gebäude per Zahl angesprochen.
     */
    int buildingIndex;
    
    /**
     * @brief Name des Gebäudes
     */
    const char* name;
    
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
     * @param buildingIndex Index des Gebäudes
     */
    const BuildingConfig* const getConfig(int buildingIndex) {
        return configs[buildingIndex];
    }
};

#endif
