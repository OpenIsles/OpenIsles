#ifndef _ABSTRACT_GRAPHICS_MGR_H
#define _ABSTRACT_GRAPHICS_MGR_H

#include <string>
#include <unordered_map>
#include "global.h"
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
    ConfigMgr* const configMgr;

    std::unordered_map<std::string, const GraphicSet*> graphicSets;

public:
    /**
     * @brief Konstruktor. Zum Laden der Grafiken muss anschließend `loadGraphics()` aufgerufen werden.
     * @param renderer (Dependency)
     * @param configMgr (Dependency)
     */
    AbstractGraphicsMgr(IRenderer* const renderer, ConfigMgr* const configMgr) :
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
    const GraphicSet* getGraphicSet(const std::string& graphicSetName) const {
        auto iter = graphicSets.find(graphicSetName);
        if (iter != graphicSets.cend()) {
            return iter->second;
        }

        Log::error(_("Count not find graphicSet '%s'."), graphicSetName.c_str());
        throw std::runtime_error("Could not find graphicSet");
    }

    IRenderer* const getRenderer() const {
        return renderer;
    }

protected:
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
     * @brief Lädt die Grafiken, die über die `tiles.xml` konfiguriert sind
     * @param configFilePath Dateipfad der Config-Datei
     */
    void loadGraphicsFromXmlConfig(const std::string& configFilePath);

    /**
     * @brief Lädt eine statische Grafik
     * @param graphicSet GraphicSet, was die Grafik erhalten soll
     * @param graphicFilename Dateipfad der Quellgrafik
     */
    void loadStaticGraphicSet(GraphicSet* graphicSet, const char* graphicFilename);

    /**
     * @brief Lädt Grafiken für ein Tileset von Straßen.
     * @param streetTileset Tilesetname
     */
    void loadStreetTileset(const std::string& streetTileset);

    /**
     * @brief Lädt eine Grafik, die horizontal-gekachelt 4 Ansichten ("south", "east", "north" und "west") enthält.
     *
     * @param graphicSet GraphicSet, was die Grafik erhalten soll
     * @param graphicFilename Dateipfad der Quellgrafik
     */
    void loadStaticGraphicSetWith4Views(GraphicSet* graphicSet, const char* graphicFilename);

    /**
     * @brief Lädt eine Harvestable-Grafik für erntebare Landschaft. Die Grafik enthält vertikal-gekachelt
     * 4 Ansichten ("south", "east", "north" und "west"), sowie horizonal-gekachelt `statesCount` verschiedene
     * Wachstums-Zustände.
     *
     * @param graphicSet GraphicSet, was die Grafik erhalten soll
     * @param graphicFilename Dateipfad der Quellgrafik
     * @param statesCount Anzahl der Wachstum-Zustände
     */
    void loadHarvestablesGraphicSet(GraphicSet* graphicSet, const char* graphicFilename, int statesCount);

    /**
     * @brief Lädt das Grafik-Set für das MapRotate-Widget
     */
    void loadMapRotateGraphicSet();

    /**
     * @brief Lädt das Grafik-Set für das MapZoom-Widget
     */
    void loadMapZoomGraphicSet();

    /**
     * @brief Lädt das Grafik-Set für die Träger
     */
    void loadCarrierGraphicSet();

    /**
     * @brief Lädt das Grafik-Set für die Marktkarren
     */
    void loadCartGraphicSet();

    /**
     * @brief Lädt das Grafik-Set für die Schafe
     */
    void loadSheepGraphicSet();

    /**
     * @brief Lädt die Grafik-Sets für die Rinder
     */
    void loadCattleGraphicSet();

    /**
     * @brief Wird aufgerufen, nachdem alle Grafiken geladen sind. Diese Methode verlinkt die GraphicSet zu den
     * MapObjectType::graphicSet
     */
    void linkGraphicSetsToMapObjectTypes();
};

#endif
