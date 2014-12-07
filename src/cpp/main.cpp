#ifdef WINDOWS
#define SDL_MAIN_HANDLED
#endif
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include "Context.h"
#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Game.h"
#include "game/GameIO.h"
#include "gui/FontMgr.h"
#include "gui/GuiMgr.h"
#include "map/Map.h"
#include "sound/SoundMgr.h"
#include "utils/FpsCounter.h"
#include "utils/StringFormat.h"

#ifdef DEBUG_A_STAR
#include "pathfinding/AStar.h"
#endif

static SDL_Color colorWhite = {255, 255, 255, 255};


/*********************************************************************************************************************
 * globale Variablen                                                                                                 *
 *********************************************************************************************************************/

#ifdef DEBUG
/**
 * @brief Zeichenketten-Puffer für 7 Zeilen Debug-Ausgabe
 */
static std::string debugOutput[7];
#endif

/*********************************************************************************************************************
 * Prototypen                                                                                                        *
 *********************************************************************************************************************/

int main(int argc, char** argv);
void drawFrame(const Context& context, SDL_Renderer* renderer);

/*********************************************************************************************************************
 * Implementierung                                                                                                   *
 *********************************************************************************************************************/

void drawFrame(const Context& context, SDL_Renderer* renderer) {
    Map* map = context.game->getMap();

	// Karte rendern
    map->renderMap(renderer);

	// UI rendern
    context.guiMgr->render(renderer);
    
    // Statuszeile
    const MapObject* selectedMapObject = map->getSelectedMapObject();
    if (selectedMapObject != nullptr) {
        const Building* selectedBuilding = reinterpret_cast<const Building*>(selectedMapObject);
        if (selectedBuilding != nullptr) {
            const BuildingConfig* buildingConfig =
                context.configMgr->getBuildingConfig(selectedBuilding->getStructureType());
            context.fontMgr->renderText(renderer, buildingConfig->name, 753, 744,
                &colorWhite, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_RIGHT);
        }
    }
    
    // Minimap auf die GUI rendern
	map->renderMinimap(renderer);

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

int main(int argc, char** argv) {
	// Library-Initialisierung ///////////////////////////////////////////////////////////////////////////////////////

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
		std::cerr << "SDL could not be initialized: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	atexit(SDL_Quit);
	std::cout << "SDL initialisert" << std::endl;

	SDL_Surface* surfaceAppIcon = IMG_Load("data/img/appicon.png");
	if (surfaceAppIcon == nullptr) {
		std::cerr << "Could not load graphic 'data/img/appicon.png': " << IMG_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow(
        "OpenIsles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "SDL could not create window: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_SetWindowIcon(window, surfaceAppIcon);
	SDL_FreeSurface(surfaceAppIcon);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (renderer == nullptr) {
		std::cerr << "SDL could not create renderer: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		return EXIT_FAILURE;
	}

	SDL_Texture* offscreenTexture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		std::cerr << "Could not init SDL-image: " << IMG_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	atexit(IMG_Quit);

	if (TTF_Init() != 0) {
		std::cerr << "Could not init SDL-TTF: " << TTF_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	atexit(TTF_Quit);

	// Game-Initialisierung //////////////////////////////////////////////////////////////////////////////////////////

    Context context = Context();

	SoundMgr* soundMgr = new SoundMgr();
    context.soundMgr = soundMgr;

    ConfigMgr* configMgr = new ConfigMgr();
    context.configMgr = configMgr;

	GraphicsMgr* graphicsMgr = new GraphicsMgr(renderer, configMgr);
    context.graphicsMgr = graphicsMgr;

    FontMgr* fontMgr = new FontMgr();
    context.fontMgr = fontMgr;

    GuiMgr* guiMgr = new GuiMgr(&context, renderer);
    context.guiMgr = guiMgr;
    guiMgr->initGui();

    EconomicsMgr* economicsMgr = new EconomicsMgr(&context);
    context.economicsMgr = economicsMgr;

    FpsCounter* fpsCounter = new FpsCounter(500);
    
    Game* game = new Game(&context);
    context.game = game;

    GameIO::loadGameFromTMX(game, "data/map/empty-map.tmx");

	// Mainloop //////////////////////////////////////////////////////////////////////////////////////////////////////

    Map* map = game->getMap();
	while (!guiMgr->hasToQuitGame()) {
		fpsCounter->startFrame();

        // während eines Frames nur einmal auf die Uhr gucken, damit z.B. beim Blinkend-Zeichnen nicht ein Schnibbel
        // eines Gebäude gezeichnet und ein anderer ne Millisekunde später nicht mehr
        context.sdlTicks = SDL_GetTicks();

		// Events handeln
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
            guiMgr->onEvent(event);
		}

        // Wirtschaft ankurbeln ;-)
        std::list<MapObject*> mapObjects = map->getMapObjects();
        for (auto iter = mapObjects.rbegin(); iter != mapObjects.rend(); iter++) {
            MapObject* mapObject = *iter;
            Structure* structure = dynamic_cast<Structure*>(mapObject);

            if (structure != nullptr) {
                economicsMgr->update(structure);
            }
        }

        // Position des Mauszeigers holen
        SDL_GetMouseState(&context.mouseCurrentX, &context.mouseCurrentY);

#ifdef DEBUG
        int screenOffsetX = map->getScreenOffsetX();
        int screenOffsetY = map->getScreenOffsetY();
        int screenZoom = map->getScreenZoom();

        int screenX, screenY;
        screenX = (context.mouseCurrentX * screenZoom) + screenOffsetX;
        screenY = (context.mouseCurrentY * screenZoom) + screenOffsetY;

        int mouseCurrentMapX, mouseCurrentMapY;
        MapCoordUtils::getMapCoordsUnderMouse(
            map, context.mouseCurrentX, context.mouseCurrentY, mouseCurrentMapX, mouseCurrentMapY);

		// Debug-Infos vorbereiten, damit wir sie später einfach nur ausgeben können
		debugOutput[0] = "FPS: average = " + toString(fpsCounter->getFpsAvg()) +
                ", current = " + toString(fpsCounter->getFpsCurrent());
        
        debugOutput[1] = "Screen: offset = (" +
                toString(screenOffsetX) + ", " + toString(screenOffsetY) + "), zoom = " +
                toString(screenZoom);

        debugOutput[2] = "mouse = (" + 
                toString(context.mouseCurrentX) + ", " + toString(context.mouseCurrentY) + "), mapElevated = (" +
                toString(mouseCurrentMapX) + ", " + toString(mouseCurrentMapY) + "), screen = (" +
                toString(screenX) + ", " + toString(screenY) + ")";
    
        if (map->getSelectedMapObject() != nullptr) {
            int mapX, mapY, mapWidth, mapHeight;
            map->getSelectedMapObject()->getMapCoords(mapX, mapY, mapWidth, mapHeight);
            
            debugOutput[3] = "selectedMapObject on mapCoords (" + 
                    toString(mapX) + ", " + toString(mapY) + "), size = (" +
                    toString(mapWidth) + ", " + toString(mapHeight) + ")";
        } else {
            debugOutput[3] = "";
        }

#ifdef DEBUG_A_STAR
        std::string buildingToUseCatchmentAreaString;
        if (AStar::debugAStar_buildingToUseCatchmentArea != nullptr) {
            int mapX, mapY;
            AStar::debugAStar_buildingToUseCatchmentArea->getMapCoords(mapX, mapY);

            buildingToUseCatchmentAreaString = "(" + toString(mapX) + ", " + toString(mapY) + ")";
        } else {
            buildingToUseCatchmentAreaString = "nullptr";
        }

        debugOutput[4] = "debugAStar: source = (" +
            toString(AStar::debugAStar_source.mapX) + ", " +
            toString(AStar::debugAStar_source.mapY) + "), destination = (" +
            toString(AStar::debugAStar_destination.mapX) + ", " +
            toString(AStar::debugAStar_destination.mapY) + "), catchmentAreaBuilding = " +
            buildingToUseCatchmentAreaString + ", useStreetOnly = " +
            (AStar::debugAStar_useStreetOnly ? "true" : "false");
#endif // DEBUG_A_STAR
#endif // DEBUG

		// Frame auf Offscreen-Texture zeichnen
		SDL_SetRenderTarget(renderer, offscreenTexture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255);
		SDL_RenderClear(renderer);
		drawFrame(context, renderer);

		// Frame fertig. Erst jetzt komplett in Fenster übertragen, um Flackern zu unterbinden
		SDL_SetRenderTarget(renderer, nullptr);
		SDL_RenderCopy(renderer, offscreenTexture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		fpsCounter->endFrame();
	}

	// Game-Deinitialisierung ////////////////////////////////////////////////////////////////////////////////////////
    
    delete game;
    
    delete fpsCounter;
    delete economicsMgr;
	delete guiMgr;
    delete fontMgr;
	delete graphicsMgr;
    delete configMgr;
	delete soundMgr;

	// Library-Deinitialisierung /////////////////////////////////////////////////////////////////////////////////////

	SDL_DestroyTexture(offscreenTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
