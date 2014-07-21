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
	// Bildfläche leermachen
	SDL_RenderClear(renderer);

	// Karte rendern
	map->renderMap(renderer);

	// FPS rendern
	std::string fpsString = "FPS = " + std::to_string(fps);
	renderText(renderer, fpsString, 10, 10);

	// Debugging-Infos rendern
	std::string debugInfosString = "MousePos = (" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + ")";
	renderText(renderer, debugInfosString, 10, 25);

	// Bildfläche anzeigen
	SDL_RenderPresent(renderer);
}

int main(int argc, char** argv) {
	// Library-Initialisierung ///////////////////////////////////////////////////////////////////////////////////////

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		std::cerr << "SDL could not be initialized: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	atexit(SDL_Quit);
	std::cout << "SDL initialisert" << std::endl;

	SDL_Window* window = SDL_CreateWindow("OpenIsles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768,
			SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "SDL could not create window: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cerr << "SDL could not create renderer: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		return EXIT_FAILURE;
	}

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

		// Frame zeichnen
		drawFrame(renderer);

		// FPS berechnen
		Uint32 ticksAtLoopEnd = SDL_GetTicks();
		fps = 1000.0 / (ticksAtLoopEnd - ticksAtLoopStart);
	}

	// Game-Deinitialisierung ////////////////////////////////////////////////////////////////////////////////////////

	delete map;
	delete graphicsMgr;

	// Library-Deinitialisierung /////////////////////////////////////////////////////////////////////////////////////

	TTF_CloseFont(ttfFont);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
