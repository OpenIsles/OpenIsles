#ifndef _ABSTRACT_GRAPHICS_MGR_H
#define _ABSTRACT_GRAPHICS_MGR_H

#include <string>
#include <unordered_map>
#include "config/ConfigMgr.h"
#include "game/Player.h"
#include "graphics/graphic/IGraphic.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "graphics/renderer/IRenderer.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"



/**
 * @brief Abstrakter Grafik-Manager, der die gemeinsame Funktionalität zwischen dem
 * `SDLGraphicsMgr` und dem `NoSDLGraphicsMgr` enthält.
 */
class AbstractGraphicsMgr : public IGraphicsMgr {

protected:
    // Dependencies
    IRenderer* const renderer;
    const ConfigMgr* const configMgr;

    std::unordered_map<std::string, const GraphicSet*> graphicSets;

public:
    /**
     * @brief Konstruktor. Zum Laden der Grafiken muss anschließend `loadGraphics()` aufgerufen werden.
     * @param renderer (Dependency)
     * @param configMgr (Dependency)
     */
    AbstractGraphicsMgr(IRenderer* const renderer, const ConfigMgr* const configMgr) :
        renderer(renderer), configMgr(configMgr) {}

    /**
     * @brief Entlädt die Grafiken und gibt den Speicher wieder frei.
     */
    virtual ~AbstractGraphicsMgr();

    void loadGraphics();


    /**
     * @brief Liefert ein bestimmtes Grafik-Set zurück.
     * @param graphicName Nach des Grafik-Sets
     * @return Grafik-Set
     */
    const GraphicSet* getGraphicSet(std::string graphicSetName) const {
        return graphicSets.at(graphicSetName);
    }

    IRenderer* const getRenderer() const {
        return renderer;
    }

protected:
    virtual void loadStaticGraphicSet(const std::string&, const char* graphicFilename);
    virtual void loadStaticGraphicSet(const std::string&, const char* graphicFilename, unsigned char mapWidth, unsigned char mapHeight);
    virtual void loadStaticAnimationGraphicSet(
        const std::string& graphicSetName, const char* graphicFilename,
        unsigned char mapWidth, unsigned char mapHeight, unsigned int countFrames);

    /**
     * @brief Lädt eine Grafik ohne Größen-Entsprechung in Kachel-Koordinaten
     * Implementierungen wählen die entsprechende Grafik-Implementierung.
     *
     * @param filename Dateiname der zu ladenden Grafik
     */
    virtual IGraphic* loadGraphic(const char* filename) = 0;

    /**
     * @brief Lädt eine Grafik.
     * Implementierungen wählen die entsprechende Grafik-Implementierung.
     *
     * @param filename Dateiname der zu ladenden Grafik
     * @param mapWidth Breite der Grafik in Map-Koordinaten
     * @param mapHeight Höhe der Grafik in Map-Koordinaten
     */
    virtual IGraphic* loadGraphic(const char* filename, unsigned char mapWidth, unsigned char mapHeight) = 0;

    /**
     * @brief Erzeugt eine Grafik aus einem Ausschnitt einer anderen Gragfik.
     * Implementierungen wählen die entsprechende Grafik-Implementierung.
     *
     * @param srcGraphic Quellgrafik
     * @param srcRect Rechteck innerhalb der Quellgrafik, die den Bereich markiert, welcher die neue Grafik auszeichnet
	 * @param mapWidth Breite der Grafik in Map-Koordinaten
	 * @param mapHeight Höhe der Grafik in Map-Koordinaten
     */
    virtual IGraphic* loadGraphic(
        const IGraphic& srcGraphic, const Rect& srcRect, unsigned char mapWidth, unsigned char mapHeight) = 0;

private:
    /**
	 * @brief Lädt die Tile-Grafiken
     */
	void loadTiles();

    /**
     * @brief Lädt die Grafiken für die Straßen
     */
    void loadStreets();

    /**
     * @brief Lädt eine Grafik, die horizontal-gekachelt 4 Ansichten ("south", "east", "north" und "west") enthält.
     *
     * @param graphicSetName GrafikSet-Name
     * @param graphicFilename Quellgrafik
	 * @param mapWidth Breite der Grafik in Map-Koordinaten
	 * @param mapHeight Höhe der Grafik in Map-Koordinaten
     */
    void loadStaticGraphicSetWith4Views(
        const std::string& graphicSetName, const char* graphicFilename, unsigned char mapWidth, unsigned char mapHeight);
};

#endif
