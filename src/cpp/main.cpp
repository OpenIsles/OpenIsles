#ifdef WINDOWS
#define SDL_MAIN_HANDLED
#endif
#include <cstdlib>
#include <getopt.h>
#include <libintl.h>
#include <stdexcept>
#include "global.h"
#include "ai/AiMgr.h"
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

bool parseCmdlineParams(Context& context, int argc, char** argv) {
    static struct option allowedLongOptions[] = {
        { "benchmark-frames", required_argument, 0, 'b' },
        { "lua-console",      optional_argument, 0, 'l' },
        { "map",              required_argument, 0, 'm' },
        { "screenshot",       required_argument, 0, 's' },
        { "ticks-to-run",     required_argument, 0, 't' },
        { nullptr, 0, 0, 0 }
    };

    std::string usageMessage = _(
            "\nUsage: OpenIsles -m file [-b frames] [-t ticks -s file]\n\n"
            "  -b, --benchmark       If specified only a certain amount of frames are\n"
            "                        executed, then the game ends automatically. This option\n"
            "                        is used for performance evaluations.\n"
            "  -l, --lua-console     Enables the in-game Lua console.\n"
            "  -m, --map             Map file to load. This parameter is required.\n"
            "  -s, --screenshot      Creates a big screenshot and exits right away. This\n"
            "                        option specifies which file the screenshot is saved to.\n"
            "                        Use this option with -t to play the game forward a\n"
            "                        certain amount of time before taking the screenshot.\n"
            "  -t, --ticks-to-run    Used in combination with -s to delay the screenshot.\n"
            "                        This option supplies the ticks (= milliseconds game\n"
            "                        time) to run.\n"
    );

    int opt;
    int longIndex = 0;
    while ((opt = getopt_long(argc, argv, "b:lm:s:t:", allowedLongOptions, &longIndex)) != -1) {
        switch (opt) {
            case 'b':
                context.cmdlineParams.benchmarkFrames = atoi(optarg);
                break;

            case 'l':
                context.cmdlineParams.enableLuaConsole = true;
                break;

            case 'm':
                context.cmdlineParams.mapFileToLoad = optarg;
                break;

            case 's':
                context.cmdlineParams.bmpFileForBigScreenshot = optarg;
                break;

            case 't':
                context.cmdlineParams.ticksToRun = (unsigned long) atol(optarg);
                break;

            default:
                Log::error(usageMessage);
                return false;
        }
    }

    if (context.cmdlineParams.mapFileToLoad == nullptr) {
        Log::error(_("Error: No map file specified."));
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    Context context = Context();

    // Internationalisierung aktivieren //////////////////////////////////////////////////////////////////////////////

    LanguageMgr::initFromEnvironment();

    // Befehlszeilen-Analyse /////////////////////////////////////////////////////////////////////////////////////////
    if (!parseCmdlineParams(context, argc, argv)) {
        return EXIT_FAILURE;
    }

    // Library-Initialisierung ///////////////////////////////////////////////////////////////////////////////////////

    IRenderer* sdlRenderer = new SDLRenderer();

    // Game-Initialisierung //////////////////////////////////////////////////////////////////////////////////////////

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

    AiMgr* aiMgr = new AiMgr(context);
    aiMgr->loadScript("data/ai/simple-ai/main.lua");

#ifdef DEBUG
    PerformanceCounter performanceCounterEvents(500);
    PerformanceCounter performanceCounterGameUpdate(500);
    PerformanceCounter performanceCounterRendering(500);
    PerformanceCounter performanceCounterAi(500);
#endif
    
    Game* game = new Game(context);
    context.game = game;

    game->loadGameFromTMX(context.cmdlineParams.mapFileToLoad);
    aiMgr->init();

    sdlRenderer->showWindow();

    // Automatischer Riesenscreenshot angefordert? ///////////////////////////////////////////////////////////////////

    if ((context.cmdlineParams.ticksToRun > 0) && (context.cmdlineParams.bmpFileForBigScreenshot != nullptr)) {
        game->update(context.cmdlineParams.ticksToRun, 1000);
        guiMgr->takeScreenshot(true, context.cmdlineParams.bmpFileForBigScreenshot);

        guiMgr->quit();
    }

    // Mainloop //////////////////////////////////////////////////////////////////////////////////////////////////////

    int benchmarkFramesToGo = context.cmdlineParams.benchmarkFrames;

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

        // KI
#ifdef DEBUG
        performanceCounterAi.start();
#endif
        aiMgr->update();

#ifdef DEBUG
        performanceCounterAi.end();
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
              "gameUpdate avg %.3f, current = %.1f; rendering avg %.3f, current = %.1f; "
              "AI avg %.3f, current = %.1f"),
            performanceCounterEvents.getMillisAvg(),
            performanceCounterEvents.getMillisCurrent(),
            performanceCounterGameUpdate.getMillisAvg(),
            performanceCounterGameUpdate.getMillisCurrent(),
            performanceCounterRendering.getMillisAvg(),
            performanceCounterRendering.getMillisCurrent(),
            performanceCounterAi.getMillisAvg(),
            performanceCounterAi.getMillisCurrent()
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

    delete aiMgr;
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
