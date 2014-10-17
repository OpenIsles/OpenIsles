#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include "config/BuildingConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Game.h"
#include "game/GameIO.h"
#include "game/Player.h"
#include "graphics/GraphicsMgr.h"
#include "gui/FontMgr.h"
#include "gui/GuiMgr.h"
#include "sound/SoundMgr.h"
#include "utils/FpsCounter.h"

static SDL_Color colorWhite = {255, 255, 255, 255};


/*********************************************************************************************************************
 * globale Variablen                                                                                                 *
 *********************************************************************************************************************/

/**
 * @brief SDL-Renderer
 */
SDL_Renderer* renderer;

/**
 * @brief Grafik-Manager
 */
GraphicsMgr* graphicsMgr;

/**
 * @brief aktuelle Position des Mauszeigers
 */
int mouseCurrentX, mouseCurrentY;

/**
 * @brief aktuelle Map-Koordinaten unter dem Mauszeiger
 */
int mouseCurrentMapX, mouseCurrentMapY;

/**
 * @brief FPS-Counter
 */
FpsCounter* fpsCounter;

/**
 * @brief Zeichenketten-Puffer für 7 Zeilen Debug-Ausgabe
 */
std::string debugOutput[7];

/**
 * @brief Verwaltung der Schriftarten
 */
FontMgr* fontMgr;

/**
 * @brief die Benutzeroberfläche
 */
GuiMgr* guiMgr;

/**
 * @brief der Sound-Manager
 */
SoundMgr* soundMgr;

/**
 * @brief Die Konfiguration der Gebäude
 */
BuildingConfigMgr* buildingConfigMgr;

/**
 * @brief Zustand des Spiels
 */
Game* game;

/**
 * @brief Manager zuständig für die Wirtschaft
 */
EconomicsMgr* economicsMgr;

/**
 * @brief Größe des Fensters
 */
extern const int windowWidth = 1024;
extern const int windowHeight = 768;

/**
 * @brief Bool-Flag, um die Mainloop zu breaken und das Spiel zu beenden
 */
bool quitGame = false;

/*********************************************************************************************************************
 * Prototypen                                                                                                        *
 *********************************************************************************************************************/

int main(int argc, char** argv);
void drawFrame(SDL_Renderer* renderer);

/*********************************************************************************************************************
 * Implementierung                                                                                                   *
 *********************************************************************************************************************/

void drawFrame(SDL_Renderer* renderer) {
    Map* map = game->getMap();

	// Karte rendern
    map->renderMap(renderer);
    
    // Resourcen oben an die Karte ran
    game->renderResourcesBar();

	// UI rendern
	guiMgr->render(renderer);
    
    // Statuszeile
    const MapObject* selectedMapObject = map->getSelectedMapObject();
    if (selectedMapObject != nullptr) {
        const Building* selectedBuilding = reinterpret_cast<const Building*>(selectedMapObject);
        if (selectedBuilding != nullptr) {
            const BuildingConfig* buildingConfig = buildingConfigMgr->getConfig(selectedBuilding->getStructureType());
            fontMgr->renderText(renderer, buildingConfig->name, 753, 744,
                &colorWhite, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_RIGHT);
        }
    }
    
    // Minimap auf die GUI rendern
	map->renderMinimap(renderer);

	// Debugging-Infos rendern
	for (int i = 0; i < 7; i++) {
		if (debugOutput[i].empty()) {
			continue;
		}

		fontMgr->renderText(renderer, debugOutput[i], 10, 40 + 15 * i,
            &colorWhite, nullptr, "DroidSans-Bold.ttf", 14, RENDERTEXT_HALIGN_LEFT);
	}
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

	SDL_Window* window = SDL_CreateWindow("OpenIsles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth,
			windowHeight, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "SDL could not create window: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_SetWindowIcon(window, surfaceAppIcon);
	SDL_FreeSurface(surfaceAppIcon);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (renderer == nullptr) {
		std::cerr << "SDL could not create renderer: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		return EXIT_FAILURE;
	}

	SDL_Texture* offscreenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			windowWidth, windowHeight);

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

	soundMgr = new SoundMgr();
    buildingConfigMgr = new BuildingConfigMgr();
	graphicsMgr = new GraphicsMgr();
    fontMgr = new FontMgr();
    guiMgr = new GuiMgr();
    fpsCounter = new FpsCounter(500);
    
    game = new Game();
    GameIO::loadGameFromTMX("data/map/saved-game.tmx");

	// Mainloop //////////////////////////////////////////////////////////////////////////////////////////////////////

    Map* map = game->getMap();
	while (!quitGame) {
		fpsCounter->startFrame();

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
        SDL_GetMouseState(&mouseCurrentX, &mouseCurrentY);

		// Debug-Infos vorbereiten, damit wir sie später einfach nur ausgeben können
		debugOutput[0] = "FPS: average = " + std::to_string(fpsCounter->getFpsAvg()) + 
                ", current = " + std::to_string(fpsCounter->getFpsCurrent());
        
        int screenOffsetX = map->getScreenOffsetX();
        int screenOffsetY = map->getScreenOffsetY();
        int screenZoom = map->getScreenZoom();
        debugOutput[1] = "Screen: offset = (" + 
                std::to_string(screenOffsetX) + ", " + std::to_string(screenOffsetY) + "), zoom = " +
                std::to_string(screenZoom);
        
        int screenX, screenY;
        screenX = (mouseCurrentX * screenZoom) + screenOffsetX;
        screenY = (mouseCurrentY * screenZoom) + screenOffsetY;
        MapUtils::screenToMapCoords(screenX, screenY, mouseCurrentMapX, mouseCurrentMapY);
        
        debugOutput[2] = "mouse = (" + 
                std::to_string(mouseCurrentX) + ", " + std::to_string(mouseCurrentY) + "), map = (" +
                std::to_string(mouseCurrentMapX) + ", " + std::to_string(mouseCurrentMapY) + "), screen = (" +
                std::to_string(screenX) + ", " + std::to_string(screenY) + ")";
    
        if (map->getSelectedMapObject() != nullptr) {
            int mapX, mapY, mapWidth, mapHeight;
            int screenX, screenY, screenWidth, screenHeight;
            
            map->getSelectedMapObject()->getMapCoords(mapX, mapY, mapWidth, mapHeight);
            map->getSelectedMapObject()->getScreenCoords(screenX, screenY, screenWidth, screenHeight);
            
            debugOutput[3] = "selectedMapObject on mapCoords (" + 
                    std::to_string(mapX) + ", " + std::to_string(mapY) + "), size = (" +
                    std::to_string(mapWidth) + ", " + std::to_string(mapHeight) + ")";
            
            debugOutput[4] = "selectedMapObject on screenCoords (" + 
                    std::to_string(screenX) + ", " + std::to_string(screenY) + "), size = (" + 
                    std::to_string(screenWidth) + ", " + std::to_string(screenHeight) + ")";
        } else {
            debugOutput[3] = "";
            debugOutput[4] = "";
        }

		// Frame auf Offscreen-Texture zeichnen
		SDL_SetRenderTarget(renderer, offscreenTexture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255);
		SDL_RenderClear(renderer);
		drawFrame(renderer);

		// Frame fertig. Erst jetzt komplett in Fenster übertragen, um Flackern zu unterbinden
		SDL_SetRenderTarget(renderer, nullptr);
		SDL_RenderCopy(renderer, offscreenTexture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		fpsCounter->endFrame();
	}

	// Game-Deinitialisierung ////////////////////////////////////////////////////////////////////////////////////////
    
    delete game;
    
    delete fpsCounter;
	delete guiMgr;
    delete fontMgr;
	delete graphicsMgr;
    delete buildingConfigMgr;
	delete soundMgr;

	// Library-Deinitialisierung /////////////////////////////////////////////////////////////////////////////////////

	SDL_DestroyTexture(offscreenTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
