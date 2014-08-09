#ifndef _BUILDING_CONFIG_MGR_H
#define _BUILDING_CONFIG_MGR_H


enum StructureType : unsigned char;

/**
 * @brief Hilfsklasse, die Daten innerhalb eines Rechtecks mit definierter Breite und Höhe hält.
 */
template <typename T>
struct RectangleData {
    /**
     * Breite des Rechtecks
     */
    int width;
    
    /**
     * Höhe des Rechtecks
     */
    int height;
    
    /**
     * Nutzlast
     */
    T* data;
    
    RectangleData(int width, int height) : width(width), height(height) {
        data = new T[width * height];
    }
    
    ~RectangleData() {
        delete[] data;
    }  
    
};

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
    RectangleData<char>* GetCatchmentArea() const {
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
