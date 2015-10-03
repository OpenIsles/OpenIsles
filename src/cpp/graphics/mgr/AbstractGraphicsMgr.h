#ifndef _ABSTRACT_GRAPHICS_MGR_H
#define _ABSTRACT_GRAPHICS_MGR_H

#include <string>
#include <unordered_map>
#include "defines.h"
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
    VIRTUAL_ONLY_IN_TESTS
    const GraphicSet* getGraphicSet(std::string graphicSetName) const {
        return graphicSets.at(graphicSetName);
    }

    IRenderer* const getRenderer() const {
        return renderer;
    }

protected:
    virtual void loadStaticGraphicSet(const std::string&, const char* graphicFilename);
    virtual void loadStaticAnimationGraphicSetWith8Views(
        const std::string& graphicSetName, const char* graphicFilename, unsigned int countFrames);

    /**
     * @brief Lädt eine Grafik.
     * Implementierungen wählen die entsprechende Grafik-Implementierung.
     *
     * @param filename Dateiname der zu ladenden Grafik
     */
    virtual IGraphic* loadGraphic(const char* filename) = 0;

    /**
     * @brief Erzeugt eine Grafik aus einem Ausschnitt einer anderen Gragfik.
     * Implementierungen wählen die entsprechende Grafik-Implementierung.
     *
     * @param srcGraphic Quellgrafik
     * @param srcRect Rechteck innerhalb der Quellgrafik, die den Bereich markiert, welcher die neue Grafik auszeichnet
     */
    virtual IGraphic* loadGraphic(const IGraphic& srcGraphic, const Rect& srcRect) = 0;

private:
    /**
     * @brief Lädt die Tile-Grafiken
     */
    void loadTiles();

    /**
     * @brief Lädt Grafiken für ein Tileset von Straßen.
     * @param streetTileset Tilesetname
     */
    void loadStreetTileset(const std::string& streetTileset);

    /**
     * @brief Lädt eine Grafik, die horizontal-gekachelt 4 Ansichten ("south", "east", "north" und "west") enthält.
     *
     * @param graphicSetName GrafikSet-Name
     * @param graphicFilename Quellgrafik
     */
    void loadStaticGraphicSetWith4Views(const std::string& graphicSetName, const char* graphicFilename);

    /**
     * @brief Lädt eine Harvestable-Grafik für erntebare Landschaft. Die Grafik enthält vertikal-gekachelt
     * 4 Ansichten ("south", "east", "north" und "west"), sowie horizonal-gekachelt `tileStatesCount` verschiedene
     * Wachstums-Zustände.
     *
     * @param graphicSetName GrafikSet-Name
     * @param graphicFilename Quellgrafik
     * @param tileStatesCount Anzahl der Wachstum-Zustände
     */
    void loadHarvestablesGraphicSet(const std::string& graphicSetName, const char* graphicFilename, int tileStatesCount);

    /**
     * @brief Lädt das Grafik-Set für das MapRotate-Widget
     */
    void loadMapRotateGraphicSet();

    /**
     * @brief Lädt das Grafik-Set für das MapZoom-Widget
     */
    void loadMapZoomGraphicSet();
};

#endif
