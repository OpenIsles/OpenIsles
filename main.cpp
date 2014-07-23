#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include "GraphicsMgr.h"
#include "Map.h"

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
 * @brief TTF-Schriftart zum Render der FPS
 */
TTF_Font* ttfFont;

/**
 * @brief aktuelle FPS des letzten Frames
 */
double fps = 0.0;

/**
 * @brief Zeichenketten-Puffer für 7 Zeilen Debug-Ausgabe
 */
std::string debugOutput[7];

/**
 * @brief aktuelle Position des Mauszeigers (X-Koordinate)
 */
int mouseX = -1;

/**
 * @brief aktuelle Position des Mauszeigers (Y-Koordinate)
 */
int mouseY = -1;

/**
 * @brief die Karte
 */
Map* map;

/*********************************************************************************************************************
 * Prototypen                                                                                                        *
 *********************************************************************************************************************/

int main(int argc, char** argv);
void renderText(SDL_Renderer* renderer, std::string string, int x, int y);
void drawFrame(SDL_Renderer* renderer);

/*********************************************************************************************************************
 * Implementierung                                                                                                   *
 *********************************************************************************************************************/

void renderText(SDL_Renderer* renderer, std::string string, int x, int y) {
	SDL_Color fpsColor = { 255, 255, 255, 0 };
	SDL_Surface* surfaceText = TTF_RenderText_Solid(ttfFont, string.data(), fpsColor);
	SDL_Rect rectDestination = { x, y, surfaceText->w, surfaceText->h };
	SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
	SDL_FreeSurface(surfaceText);
	SDL_RenderCopy(renderer, textureText, NULL, &rectDestination);
	SDL_DestroyTexture(textureText);
}

void drawFrame(SDL_Renderer* renderer) {
	// Karte rendern
	map->renderMap(renderer);

	// Debugging-Infos rendern
	for (int i = 0; i < 7; i++) {
		renderText(renderer, debugOutput[i], 10, 10 + 15 * i);
	}
}

int main(int argc, char** argv) {
	// Library-Initialisierung ///////////////////////////////////////////////////////////////////////////////////////

	const int windowWidth = 1024;
	const int windowHeight = 768;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		std::cerr << "SDL could not be initialized: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	atexit(SDL_Quit);
	std::cout << "SDL initialisert" << std::endl;

	SDL_Window* window = SDL_CreateWindow("OpenIsles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth,
			windowHeight, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "SDL could not create window: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

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

	ttfFont = TTF_OpenFont("data/font/DroidSans-Bold.ttf", 14);

	// Game-Initialisierung //////////////////////////////////////////////////////////////////////////////////////////

	graphicsMgr = new GraphicsMgr();
	map = new Map(40, 40);

	// Mainloop //////////////////////////////////////////////////////////////////////////////////////////////////////

	bool quitGame = false;
	while (!quitGame) {
		Uint32 ticksAtLoopStart = SDL_GetTicks();

		// Events handeln
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quitGame = true;
			} else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					quitGame = true;
				} else if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
					map->scroll(0, -16);
				} else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
					map->scroll(0, 16);
				} else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
					map->scroll(-16, 0);
				} else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
					map->scroll(16, 0);
				}
			}
		}

		// Mausposition auslesen
		SDL_GetMouseState(&mouseX, &mouseY);

		// Debug-Infos vorbereiten, damit wir sie später einfach nur ausgeben können
		std::string fpsString = "FPS = " + std::to_string(fps);
		debugOutput[0] = fpsString;

		std::string mousePosString = "MousePos = (" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + ")";
		debugOutput[1] = mousePosString;

		int mouseAtScreenX = mouseX + map->getScreenOffsetX();
		int mouseAtScreenY = mouseY + map->getScreenOffsetY();
		std::string mouseAtScreenString = "MouseScreenPos = (" + std::to_string(mouseAtScreenX) + ", "
				+ std::to_string(mouseAtScreenY) + ")";
		debugOutput[2] = mouseAtScreenString;

		int mouseMapX, mouseMapY;
		map->screenToMapCoords(mouseAtScreenX, mouseAtScreenY, mouseMapX, mouseMapY);
		std::string mouseMapString = "MouseMapPos = (" + std::to_string(mouseMapX) + ", " + std::to_string(mouseMapY)
				+ ")";
		debugOutput[3] = mouseMapString;

		std::list<MapObject*> mapObjects = map->getMapObjects();
		std::string boundingBoxObjectsMapString = "objectsMap = (";
		std::string boundingBoxObjectsScreenString = "objectsScreen = (";
		std::string realCollisionString = "realCollision = (";

		// Objekte rückwärts iterieren. Somit kommen "oben liegende" Objekte zuerst dran
		for (auto iter = mapObjects.crbegin(); iter != mapObjects.crend(); iter++) {
			MapObject* mapObject = *iter;

			if ((mouseMapX >= mapObject->mapX) && (mouseMapX < mapObject->mapX + mapObject->mapWidth)
					&& (mouseMapY >= mapObject->mapY) && (mouseMapY < mapObject->mapY + mapObject->mapHeight)) {

				boundingBoxObjectsMapString.append("object " + std::to_string(mapObject->object) + ", ");
			}

			if ((mouseAtScreenX >= mapObject->screenX) && (mouseAtScreenX < mapObject->screenX + mapObject->screenWidth)
					&& (mouseAtScreenY >= mapObject->screenY)
					&& (mouseAtScreenY < mapObject->screenY + mapObject->screenHeight)) {

				boundingBoxObjectsScreenString.append("object " + std::to_string(mapObject->object) + ", ");

				Uint8 r, g, b, a;
				int x = mouseAtScreenX - mapObject->screenX;
				int y = mouseAtScreenY - mapObject->screenY;
				graphicsMgr->getObject(mapObject->object)->getPixel(x, y, &r, &g, &b, &a);

				realCollisionString.append("a = " + std::to_string(a) + ", ");
			}
		}
		boundingBoxObjectsMapString.append(")");
		boundingBoxObjectsScreenString.append(")");
		realCollisionString.append(")");
		debugOutput[4] = boundingBoxObjectsMapString;
		debugOutput[5] = boundingBoxObjectsScreenString;
		debugOutput[6] = realCollisionString;

		// Frame auf Offscreen-Texture zeichnen
		SDL_SetRenderTarget(renderer, offscreenTexture);
		SDL_RenderClear(renderer);
		drawFrame(renderer);

		// Frame fertig. Erst jetzt komplett in Fenster übertragen, um Flackern zu unterbinden
		SDL_SetRenderTarget(renderer, nullptr);
		SDL_RenderCopy(renderer, offscreenTexture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		// FPS berechnen
		Uint32 ticksAtLoopEnd = SDL_GetTicks();
		fps = 1000.0 / (ticksAtLoopEnd - ticksAtLoopStart);
	}

	// Game-Deinitialisierung ////////////////////////////////////////////////////////////////////////////////////////

	delete map;
	delete graphicsMgr;

	// Library-Deinitialisierung /////////////////////////////////////////////////////////////////////////////////////

	TTF_CloseFont(ttfFont);

	SDL_DestroyTexture(offscreenTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
