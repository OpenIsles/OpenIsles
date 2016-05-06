#ifdef WINDOWS
#define SDL_MAIN_HANDLED
#endif
#include <cstdlib>
#include <libintl.h>
#include <stdexcept>
#include <unistd.h>
#include "global.h"
#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Game.h"
#include "game/GameIO.h"
#include "graphics/mgr/sdl/SDLFontMgr.h"
#include "graphics/mgr/sdl/SDLGraphicsMgr.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "gui/GuiMgr.h"
#include "map/coords/MapCoords.h"
#include "map/coords/ScreenCoords.h"
#include "map/Map.h"
#include "sound/sdl/SDLSoundMgr.h"
#include "utils/Color.h"
#include "utils/Events.h"
#include "utils/PerformanceCounter.h"
#include "utils/RandomEngine.h"
#include "utils/StringFormat.h"

#ifdef DEBUG_A_STAR
#include "pathfinding/AStar.h"
#endif

/* Globale TODOs
 * --------------
 *
 * - TODO Fehlerhandling verbessern. Aktuell geben die meisten Fehlerfälle eine lokalisierte Meldung auf stderr aus,
 * werfen danach eine nicht-lokalisierte Exception, die niemand fängt.
 */


/*********************************************************************************************************************
 * lokale Variablen                                                                                                  *
 *********************************************************************************************************************/

#ifdef DEBUG
/**
 * @brief Zeichenketten-Puffer für 7 Zeilen Debug-Ausgabe
 */
static std::string debugOutput[7];
#endif


static PerformanceCounter fpsCounter(500);

/**
 * @brief hält die Befehlszeilen-Parameter
 */
static struct {

    /**
     * @brief wenn ungleich 0, gibt an, wie viele Frames das Spiel laufen soll, bis es automatisch beendet wird.
     * Dies ist zum Messen der Ausführungsgeschwindigkeit, um immer dieselben Bedingungen und gleiche Laufzeit
     * zu haben.
     */
    int benchmarkFrames = 0;

    /**
     * @brief Dateiname der Map, die geladen werden soll
     */
    char* mapFileToLoad = nullptr;

    /**
     * @brief wenn ungleich 0, läuft das Spiel exakt die angegebene [Spielzeit](@ref gameTicks), führt danach den
     * Riesenscreenshot aus und beendet dann.
     */
    unsigned long ticksToRun = 0;

    /**
     * @brief Dateiname, unter dem ein Riesenscreenshot abgespeichert werden soll
     */
    char* bmpFileForBigScreenshot = nullptr;

} cmdlineParams;


/*********************************************************************************************************************
 * Prototypen                                                                                                        *
 *********************************************************************************************************************/

int main(int argc, char** argv);

/**
 * @brief Analysiert die Befehlszeilen-Parameter und füllt cmdlineParams entsprechend.
 * @param argc Anzahl der Parameter
 * @param argc Zeiger auf Parameter-Array
 * @return `true`, wenn alles ok war.
 *         `false` wenn die Befehlszeile fehlerhaft war. In diesem Fall sollte der Aufrufer das Programm sofort beenden.
 */
bool parseCmdlineParams(int argc, char** argv);

void drawFrame(const Context& context, IRenderer* renderer);

/*********************************************************************************************************************
 * Implementierung                                                                                                   *
 *********************************************************************************************************************/

void drawFrame(const Context& context, IRenderer* renderer) {
    // UI rendern
    context.guiMgr->render();

    // FPS-Anzeige
    if (context.game->isFpsCounterEnabled()) {
        context.fontMgr->renderText(renderer, _("FPS:"),
                                    10, 686, &Color::white, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT);

        context.fontMgr->renderText(renderer, _("current:"),
                                    77, 698, &Color::white, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_RIGHT);
        context.fontMgr->renderText(renderer, toString((int) fpsCounter.getFpsCurrent()),
                                    80, 698, &Color::white, nullptr, "DroidSans.ttf", 14, RENDERTEXT_HALIGN_LEFT);

        context.fontMgr->renderText(renderer, _("average:"),
                                    77, 711, &Color::white, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_RIGHT);
        context.fontMgr->renderText(renderer, toString(fpsCounter.getFpsAvg()),
                                    80, 711, &Color::white, nullptr, "DroidSans.ttf", 14, RENDERTEXT_HALIGN_LEFT);
    }

#ifdef DEBUG
    // Debugging-Infos rendern
    for (int i = 0; i < 7; i++) {
        if (debugOutput[i].empty()) {
            continue;
        }

        context.fontMgr->renderText(renderer, debugOutput[i], 10, 40 + 15 * i,
            &Color::white, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT);
    }
#endif
}

bool parseCmdlineParams(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "b:m:t:s:")) != -1) {
        switch (opt) {
            case 'b':
                cmdlineParams.benchmarkFrames = atoi(optarg);
                break;

            case 'm':
                cmdlineParams.mapFileToLoad = optarg;
                break;

            case 't':
                cmdlineParams.ticksToRun = (unsigned long) atol(optarg);
                break;

            case 's':
                cmdlineParams.bmpFileForBigScreenshot = optarg;
                break;

            default:
                Log::error(_("Usage: %s -m mapFileToLoad [-b benchmarkFrames] [-t runOnlyGameTicks -s bmpFileForBigScreenshot]"),
                           argv[0]);
                return false;
        }
    }

    if (cmdlineParams.mapFileToLoad == nullptr) {
        Log::error(_("Error: No map file specified."));
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    // Internationalisierung aktivieren //////////////////////////////////////////////////////////////////////////////

    LanguageMgr::initFromEnvironment();

    // Befehlszeilen-Analyse /////////////////////////////////////////////////////////////////////////////////////////

    if (!parseCmdlineParams(argc, argv)) {
        return EXIT_FAILURE;
    }

    // Library-Initialisierung ///////////////////////////////////////////////////////////////////////////////////////

    IRenderer* sdlRenderer = new SDLRenderer();

    // Game-Initialisierung //////////////////////////////////////////////////////////////////////////////////////////

    Context context = Context();

    context.userEventBase = SDL_RegisterEvents(USER_EVENT_MAXEVENT + 1);
    if (context.userEventBase == (uint32_t) -1) {
        Log::error(_("Could not register events."));
        throw std::runtime_error("Could not register events");
    }

    IRandomEngine* randomEngine = new RealRandomEngine();
    context.randomEngine = randomEngine;

    ISoundMgr* sdlSoundMgr = new SDLSoundMgr();
    context.soundMgr = sdlSoundMgr;

    ConfigMgr* configMgr = new ConfigMgr("data/config");
    context.configMgr = configMgr;

    IGraphicsMgr* sdlGraphicsMgr = new SDLGraphicsMgr(sdlRenderer, configMgr);
    context.graphicsMgr = sdlGraphicsMgr;
    sdlGraphicsMgr->loadGraphics();

    IFontMgr* sdlFontMgr = new SDLFontMgr();
    context.fontMgr = sdlFontMgr;

    GuiMgr* guiMgr = new GuiMgr(context, sdlRenderer);
    context.guiMgr = guiMgr;
    guiMgr->initGui();

    EconomicsMgr* economicsMgr = new EconomicsMgr(context);
    context.economicsMgr = economicsMgr;

#ifdef DEBUG
    PerformanceCounter performanceCounterEvents(500);
    PerformanceCounter performanceCounterGameUpdate(500);
    PerformanceCounter performanceCounterRendering(500);
#endif
    
    Game* game = new Game(context);
    context.game = game;

    game->loadGameFromTMX(cmdlineParams.mapFileToLoad);

    sdlRenderer->showWindow();

    // Automatischer Riesenscreenshot angefordert? ///////////////////////////////////////////////////////////////////

    if ((cmdlineParams.ticksToRun > 0) && (cmdlineParams.bmpFileForBigScreenshot != nullptr)) {
        game->updateRealistically(cmdlineParams.ticksToRun);
        guiMgr->takeScreenshot(true, cmdlineParams.bmpFileForBigScreenshot);

        guiMgr->quit();
    }

    // Mainloop //////////////////////////////////////////////////////////////////////////////////////////////////////

    int benchmarkFramesToGo = cmdlineParams.benchmarkFrames;

    uint32_t sdlTicks = SDL_GetTicks();
    uint32_t lastSdlTicks = sdlTicks;

    while (!guiMgr->hasToQuitGame()) {
        fpsCounter.start();

        // Events handeln
#ifdef DEBUG
        performanceCounterEvents.start();
#endif

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            guiMgr->onEvent(event);

            // Bei User-Defined-Events übernehmen wir hier das Freigeben des Speichers. Dann können auch mehrere
            // GUI-Elemente dasselbe Event verarbeiten und streiten sich nicht, wer delete aufruft.
            if (event.type == context.userEventBase + USER_EVENT_MOUSEMOTION_MAPCOORDS) {
                delete static_cast<MouseMotionMapCoordsEvent*>(event.user.data1);
            }
        }

#ifdef DEBUG
        performanceCounterEvents.end();
#endif


        // Objekte aktualisieren
#ifdef DEBUG
        performanceCounterGameUpdate.start();
#endif

        /* Vergangene Ticks berechnen
         *
         * Info: Wrapping tritt alle ~49,7 Tage physikalische Zeit auf.
         * Die Differenz ist aber trotzdem richtig, da wir unsigned-Variablen haben.
         */
        sdlTicks = SDL_GetTicks();
        uint32_t sdlTicksElapsed = sdlTicks - lastSdlTicks;
        lastSdlTicks = sdlTicks;

        game->update(sdlTicksElapsed);

#ifdef DEBUG
        performanceCounterGameUpdate.end();
#endif


        // Position des Mauszeigers holen
        SDL_GetMouseState(&context.mouseCurrentX, &context.mouseCurrentY);

#ifdef DEBUG
        Map* map = game->getMap();
        const MapCoords& mapCoordsCentered = map->getMapCoordsCentered();
        int screenZoom = map->getScreenZoom();
        const FourthDirection& screenView = map->getScreenView();

        ScreenCoords mouseCurrentScreenCoords =
            MapCoordUtils::getScreenCoordsUnderMouse(*map, context.mouseCurrentX, context.mouseCurrentY);
        MapCoords mouseCurrentMapCoords =
            MapCoordUtils::getMapCoordsUnderMouse(*map, context.mouseCurrentX, context.mouseCurrentY);

        // Debug-Infos vorbereiten, damit wir sie später einfach nur ausgeben können
        debugOutput[0] = string_sprintf(
            _("PerfCounters: events avg %.3f, current = %.1f; "
              "gameUpdate avg %.3f, current = %.1f; rendering avg %.3f, current = %.1f"),
            performanceCounterEvents.getMillisAvg(),
            performanceCounterEvents.getMillisCurrent(),
            performanceCounterGameUpdate.getMillisAvg(),
            performanceCounterGameUpdate.getMillisCurrent(),
            performanceCounterRendering.getMillisAvg(),
            performanceCounterRendering.getMillisCurrent()
        );

        debugOutput[1] = string_sprintf(
            _("Screen: mapCentered = (%d, %d), zoom = %d, view = %d"),
            mapCoordsCentered.x(), mapCoordsCentered.y(), screenZoom, screenView
        );

        debugOutput[2] = string_sprintf(_("Game ticks: %lu"), game->getTicks());

        debugOutput[3] = string_sprintf(
            _("mouse = (%d, %d), screen = (%d, %d), mapElevated = (%d, %d)"),
            context.mouseCurrentX,
            context.mouseCurrentY,
            mouseCurrentScreenCoords.x(),
            mouseCurrentScreenCoords.y(),
            mouseCurrentMapCoords.x(),
            mouseCurrentMapCoords.y()
        );

        const MapObject* selectedMapObject = map->getSelectedMapObject();
        if (selectedMapObject != nullptr) {
            int mapWidth = selectedMapObject->getMapWidth();
            int mapHeight = selectedMapObject->getMapHeight();

            const MapObjectFixed* smoFixed = dynamic_cast<const MapObjectFixed*>(selectedMapObject);
            if (smoFixed != nullptr) {
                const MapCoords& mapCoords = smoFixed->getMapCoords();

                debugOutput[4] = string_sprintf(
                    _("selectedMapObject(Fixed) on mapCoords (%d, %d), size = (%d, %d)"),
                    mapCoords.x(), mapCoords.y(), mapWidth, mapHeight
                );
            }
            else {
                const MapObjectMoving* smoMoving = dynamic_cast<const MapObjectMoving*>(selectedMapObject);
                if (smoMoving != nullptr) {
                    const DoubleMapCoords& mapCoords = smoMoving->getMapCoords();
                    debugOutput[4] = string_sprintf(
                        _("selectedMapObject(Moving) on mapCoords (%f, %f), size = (%d, %d)"),
                        mapCoords.x(), mapCoords.y(), mapWidth, mapHeight
                    );
                }
                else {
                    debugOutput[4] = string_sprintf(
                        _("selectedMapObject(UNKNOWN!?) size = (%d, %d)"),
                        mapWidth, mapHeight
                    );
                }
            }

        } else {
            debugOutput[4] = "";
        }

#ifdef DEBUG_A_STAR
        std::string catchmentAreaIteratorString;
        if (AStar::debugAStar_catchmentAreaIterator) {
            catchmentAreaIteratorString = _("catchmentAreaIterator set");
        } else {
            catchmentAreaIteratorString = _("catchmentAreaIterator nullptr");
        }

        debugOutput[5] = string_sprintf(
            _("debugAStar: source = (%d, %d), destination = (%d, %d), "
              "%s, useStreetOnly = %s, rightAnglesOnly = %s"),
            AStar::debugAStar_source.x(),
            AStar::debugAStar_source.y(),
            AStar::debugAStar_destination.x(),
            AStar::debugAStar_destination.y(),
            catchmentAreaIteratorString.c_str(),
            (AStar::debugAStar_useStreetOnly ? _("true") : _("false")).c_str(),
            (AStar::debugAStar_rightAnglesOnly ? _("true") : _("false")).c_str()
        );
#endif // DEBUG_A_STAR
#endif // DEBUG


#ifdef DEBUG
        performanceCounterRendering.start();
#endif

        sdlRenderer->startFrame();
        drawFrame(context, sdlRenderer);
        sdlRenderer->endFrame();

#ifdef DEBUG
        performanceCounterRendering.end();
#endif

        fpsCounter.end();

        // Benchmarking: nur x Frames, dann wird automatisch beendet
        if (benchmarkFramesToGo > 0) {
            if (--benchmarkFramesToGo == 0) {
                break;
            }
        }
    }

    // Game-Deinitialisierung ////////////////////////////////////////////////////////////////////////////////////////
    
    delete game;

    delete economicsMgr;
    delete guiMgr;
    delete sdlFontMgr;
    delete sdlGraphicsMgr;
    delete configMgr;
    delete sdlSoundMgr;
    delete randomEngine;

    // Library-Deinitialisierung /////////////////////////////////////////////////////////////////////////////////////

    delete sdlRenderer;

    return EXIT_SUCCESS;
}
