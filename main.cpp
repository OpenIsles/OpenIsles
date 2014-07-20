#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char** argv) {
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

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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

	SDL_Surface* surfaceTestTile = IMG_Load("data/img/tiles/water.png");
	if (surfaceTestTile == nullptr) {
		std::cerr << "Could not load test tile: " << IMG_GetError() << std::endl;
		SDL_DestroyWindow(window);
		return EXIT_FAILURE;
	}

	int tileImgWidth = surfaceTestTile->w;
	int tileImgHeight = surfaceTestTile->h;

	SDL_Texture* textureTestTile = SDL_CreateTextureFromSurface(renderer, surfaceTestTile);
	if (textureTestTile == nullptr) {
		std::cerr << "Could not create texture for test tile: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		return EXIT_FAILURE;
	}
	SDL_FreeSurface(surfaceTestTile);

	if (TTF_Init() != 0) {
		std::cerr << "Could not init SDL-TTF: " << TTF_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	atexit(TTF_Quit);

	TTF_Font* ttfFont = TTF_OpenFont("data/font/DroidSans-Bold.ttf", 14);

	// Mainloop
	double fps = 0.0;
	bool quitGame = false;
	while (!quitGame) {
		Uint32 ticksAtLoopStart = SDL_GetTicks();

		// Events handeln
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quitGame = true;
				break;
			} else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					quitGame = true;
					break;
				}
			}
		}

		// Bildfläche leermachen
		SDL_RenderClear(renderer);

		// Kacheln rendern
		SDL_Rect rectDestination = { 0, 0, tileImgWidth, tileImgHeight };
		for (int y = 0; y < 768; y += tileImgHeight) {
			for (int x = 0; x < 1024; x += tileImgWidth) {
				rectDestination.x = x;
				rectDestination.y = y;
				SDL_RenderCopy(renderer, textureTestTile, NULL, &rectDestination);
			}
		}

		// FPS rendern
		std::string fpsString = "FPS = " + std::to_string(fps);
		SDL_Color fpsColor = { 255, 255, 255, 0 };
		SDL_Surface* surfaceFpsText = TTF_RenderText_Solid(ttfFont, fpsString.data(), fpsColor);
		rectDestination = { 10, 10, surfaceFpsText->w, surfaceFpsText->h };
		SDL_Texture* textureFpsText = SDL_CreateTextureFromSurface(renderer, surfaceFpsText);
		SDL_FreeSurface(surfaceFpsText);
		SDL_RenderCopy(renderer, textureFpsText, NULL, &rectDestination);
		SDL_DestroyTexture(textureFpsText);

		// Bildfläche anzeigen
		SDL_RenderPresent(renderer);

		// FPS berechnen
		Uint32 ticksAtLoopEnd = SDL_GetTicks();
		fps = 1000.0 / (ticksAtLoopEnd - ticksAtLoopStart);
	}

	TTF_CloseFont(ttfFont);

	SDL_DestroyTexture(textureTestTile);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
