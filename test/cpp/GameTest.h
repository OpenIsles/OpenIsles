#ifndef _GAME_TEST_H
#define _GAME_TEST_H

#include <string>
#include <gtest/gtest.h>
#include "Context.h"
#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Game.h"
#include "graphics/mgr/nosdl/NoSDLFontMgr.h"
#include "graphics/mgr/nosdl/NoSDLGraphicsMgr.h"
#include "graphics/renderer/IRenderer.h"
#include "graphics/renderer/nosdl/NoSDLRenderer.h"
#include "map/Map.h"
#include "utils/Ticks.h"

/**
 * @brief Basisklasse für alle Test-Fixtures.
 * Diese Klasse baut den Context auf und lädt unsere Testkarte.
 */
class GameTest : public testing::Test {

private:
    /**
     * Dateiname der Karte, die geladen werden soll.
     */
    std::string mapFilename;

protected:
    Context context;

    EconomicsMgr* economicsMgr;
    ConfigMgr* configMgr;
    IRenderer* noSdlRenderer;
    IGraphicsMgr* noSdlGraphicsMgr;
    IFontMgr* noSdlFontMgr;
    Game* game;

protected:
    GameTest() : GameTest("test-map.tmx") {}
    GameTest(const std::string& mapFilename) : mapFilename(mapFilename) {}

    virtual void SetUp() {
        noSdlRenderer = new NoSDLRenderer();

        configMgr = new ConfigMgr();
        context.configMgr = configMgr;

        noSdlGraphicsMgr = new NoSDLGraphicsMgr(noSdlRenderer, configMgr);
        context.graphicsMgr = noSdlGraphicsMgr;
        noSdlGraphicsMgr->loadGraphics();

        noSdlFontMgr = new NoSDLFontMgr();
        context.fontMgr = noSdlFontMgr;

        game = new Game(&context);
        context.game = game;

        economicsMgr = new EconomicsMgr(&context);
        context.economicsMgr = economicsMgr;

        context.sdlTicks = 0;
        setTicks(context.sdlTicks);

        std::string mapPath = "data/map/" + mapFilename;
        game->loadGameFromTMX(mapPath.c_str());
    }

    virtual void TearDown() {
        delete game;

        delete economicsMgr;
        delete noSdlFontMgr;
        delete noSdlGraphicsMgr;
        delete configMgr;

        delete noSdlRenderer;
    }

};

#endif