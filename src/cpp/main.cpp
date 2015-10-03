#ifdef WINDOWS
#define SDL_MAIN_HANDLED
#endif
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include "Context.h"
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
#include "utils/StringFormat.h"

#ifdef DEBUG_A_STAR
#include "pathfinding/AStar.h"
#endif

static Color colorWhite = Color(255, 255, 255, 255);


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
        context.fontMgr->renderText(renderer, "FPS:",
                                    10, 686, &colorWhite, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT);

        context.fontMgr->renderText(renderer, "current:",
                                    77, 698, &colorWhite, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_RIGHT);
        context.fontMgr->renderText(renderer, toString((int) fpsCounter.getFpsCurrent()),
                                    80, 698, &colorWhite, nullptr, "DroidSans.ttf", 14, RENDERTEXT_HALIGN_LEFT);

        context.fontMgr->renderText(renderer, "average:",
                                    77, 711, &colorWhite, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_RIGHT);
        context.fontMgr->renderText(renderer, toString(fpsCounter.getFpsAvg()),
                                    80, 711, &colorWhite, nullptr, "DroidSans.ttf", 14, RENDERTEXT_HALIGN_LEFT);
    }

#ifdef DEBUG
    // Debugging-Infos rendern
    for (int i = 0; i < 7; i++) {
        if (debugOutput[i].empty()) {
            continue;
        }

        context.fontMgr->renderText(renderer, debugOutput[i], 10, 40 + 15 * i,
            &colorWhite, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT);
    }
#endif
}

bool parseCmdlineParams(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "b:m:")) != -1) {
        switch (opt) {
            case 'b':
                cmdlineParams.benchmarkFrames = atoi(optarg);
                break;

            case 'm':
                cmdlineParams.mapFileToLoad = optarg;
                break;

            default:
                std::cerr << "Aufruf: " << argv[0] << " -m mapFileToLoad [-b benchmarkFrames]" << std::endl;
                return false;
        }
    }

    if (cmdlineParams.mapFileToLoad == nullptr) {
        std::cerr << "Fehler: Es wurde keine Map-Datei angegeben." << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
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
        std::cerr << "Could not register events" << std::endl;
        throw std::runtime_error("Could not register events");
    }

    ISoundMgr* sdlSoundMgr = new SDLSoundMgr();
    context.soundMgr = sdlSoundMgr;

    ConfigMgr* configMgr = new ConfigMgr();
    context.configMgr = configMgr;

    IGraphicsMgr* sdlGraphicsMgr = new SDLGraphicsMgr(sdlRenderer, configMgr);
    context.graphicsMgr = sdlGraphicsMgr;
    sdlGraphicsMgr->loadGraphics();

    IFontMgr* sdlFontMgr = new SDLFontMgr();
    context.fontMgr = sdlFontMgr;

    GuiMgr* guiMgr = new GuiMgr(&context, sdlRenderer);
    context.guiMgr = guiMgr;
    guiMgr->initGui();

    EconomicsMgr* economicsMgr = new EconomicsMgr(&context);
    context.economicsMgr = economicsMgr;

#ifdef DEBUG
    PerformanceCounter performanceCounterEvents(500);
    PerformanceCounter performanceCounterGameUpdate(500);
    PerformanceCounter performanceCounterRendering(500);
#endif
    
    Game* game = new Game(&context);
    context.game = game;

    game->loadGameFromTMX(cmdlineParams.mapFileToLoad);

    sdlRenderer->showWindow();

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

        MapCoords mapCoordsTopLeft, mapCoordsTopRight, mapCoordsBottomLeft, mapCoordsBottomRight;
        MapCoordUtils::getMapCoordsInScreenEdges(
            *map, mapCoordsTopLeft, mapCoordsTopRight, mapCoordsBottomLeft, mapCoordsBottomRight);

        // Debug-Infos vorbereiten, damit wir sie später einfach nur ausgeben können
        debugOutput[0] = "PerfCounters: events avg " + toString(performanceCounterEvents.getMillisAvg()) +
                         ", current = " + toString(performanceCounterEvents.getMillisCurrent()) +
                         "; gameUpdate avg " + toString(performanceCounterGameUpdate.getMillisAvg()) +
                         ", current = " + toString(performanceCounterGameUpdate.getMillisCurrent()) +
                         "; rendering avg " + toString(performanceCounterRendering.getMillisAvg()) +
                         ", current = " + toString(performanceCounterRendering.getMillisCurrent());

        debugOutput[1] = "Screen: mapCentered = (" +
            toString(mapCoordsCentered.x()) + ", " + toString(mapCoordsCentered.y()) + "), zoom = " +
            toString(screenZoom) + ", view = " + toString(screenView);

        debugOutput[2] = "ScreenEdges: topLeft = (" +
            toString(mapCoordsTopLeft.x()) + ", " + toString(mapCoordsTopLeft.y()) + "), topRight = (" +
            toString(mapCoordsTopRight.x()) + ", " + toString(mapCoordsTopRight.y()) + "), bottomLeft = (" +
            toString(mapCoordsBottomLeft.x()) + ", " + toString(mapCoordsBottomLeft.y()) + "), bottomRight = (" +
            toString(mapCoordsBottomRight.x()) + ", " + toString(mapCoordsBottomRight.y()) + ")";

        debugOutput[3] = "mouse = (" +
            toString(context.mouseCurrentX) + ", " + toString(context.mouseCurrentY) + "), screen = (" +
            toString(mouseCurrentScreenCoords.x()) + ", " + toString(mouseCurrentScreenCoords.y()) + "), mapElevated = (" +
            toString(mouseCurrentMapCoords.x()) + ", " + toString(mouseCurrentMapCoords.y()) + "), ";

        const MapObject* selectedMapObject = map->getSelectedMapObject();
        if (selectedMapObject != nullptr) {
            int mapWidth = selectedMapObject->getMapWidth();
            int mapHeight = selectedMapObject->getMapHeight();

            const MapObjectFixed* smoFixed = dynamic_cast<const MapObjectFixed*>(selectedMapObject);
            if (smoFixed != nullptr) {
                const MapCoords& mapCoords = smoFixed->getMapCoords();
                debugOutput[4] = "selectedMapObject(Fixed) on mapCoords (" +
                    toString(mapCoords.x()) + ", " + toString(mapCoords.y()) + "), size = (" +
                    toString(mapWidth) + ", " + toString(mapHeight) + ")";
            }
            else {
                const MapObjectMoving* smoMoving = dynamic_cast<const MapObjectMoving*>(selectedMapObject);
                if (smoMoving != nullptr) {
                    const DoubleMapCoords& mapCoords = smoMoving->getMapCoords();
                    debugOutput[4] = "selectedMapObject(Moving) on mapCoords (" +
                        toString(mapCoords.x()) + ", " + toString(mapCoords.y()) + "), size = (" +
                        toString(mapWidth) + ", " + toString(mapHeight) + ")";
                }
                else {
                    debugOutput[4] = "selectedMapObject(UNKNOWN!?) size = (" +
                        toString(mapWidth) + ", " + toString(mapHeight) + ")";
                }
            }

        } else {
            debugOutput[4] = "";
        }

#ifdef DEBUG_A_STAR
        std::string buildingToUseCatchmentAreaString;
        if (AStar::debugAStar_buildingToUseCatchmentArea != nullptr) {
            const MapCoords& mapCoords =
                AStar::debugAStar_buildingToUseCatchmentArea->getMapCoords();

            buildingToUseCatchmentAreaString = "(" + toString(mapCoords.x()) + ", " + toString(mapCoords.y()) + ")";
        } else {
            buildingToUseCatchmentAreaString = "nullptr";
        }

        debugOutput[5] = "debugAStar: source = (" +
            toString(AStar::debugAStar_source.x()) + ", " +
            toString(AStar::debugAStar_source.y()) + "), destination = (" +
            toString(AStar::debugAStar_destination.x()) + ", " +
            toString(AStar::debugAStar_destination.y()) + "), catchmentAreaBuilding = " +
            buildingToUseCatchmentAreaString + ", useStreetOnly = " +
            (AStar::debugAStar_useStreetOnly ? "true" : "false") + ", rightAnglesOnly = " +
            (AStar::debugAStar_rightAnglesOnly ? "true" : "false");
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

    // Library-Deinitialisierung /////////////////////////////////////////////////////////////////////////////////////

    delete sdlRenderer;

    return EXIT_SUCCESS;
}
