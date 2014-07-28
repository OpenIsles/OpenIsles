#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include "GraphicsMgr.h"
#include "map/Map.h"

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
 * @brief die Karte
 */
Map* map;

/**
 * @brief Größe des Fensters
 */
extern const int windowWidth = 1024;
extern const int windowHeight = 768;

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

	SDL_RenderSetClipRect(renderer, nullptr);
	SDL_RenderCopy(renderer, textureText, NULL, &rectDestination);

	SDL_DestroyTexture(textureText);
}

void drawFrame(SDL_Renderer* renderer) {
	// Karte rendern
	map->renderMap(renderer);

	// Debugging-Infos rendern
	for (int i = 0; i < 7; i++) {
		if (debugOutput[i].empty()) {
			continue;
		}

		renderText(renderer, debugOutput[i], 10, 10 + 15 * i);
	}
}

int main(int argc, char** argv) {
	// Library-Initialisierung ///////////////////////////////////////////////////////////////////////////////////////

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
		while (SDL_PollEvent(&event)) {
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
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				map->onClick(event.button.x, event.button.y);
			}
		}

		// Debug-Infos vorbereiten, damit wir sie später einfach nur ausgeben können
		std::string fpsString = "FPS = " + std::to_string(fps);
		debugOutput[0] = fpsString;

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
