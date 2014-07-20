#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		std::cerr << "SDL could not be initialized: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	atexit(SDL_Quit);
	std::cout << "SDL initialisert" << std::endl;

	SDL_Window* window = SDL_CreateWindow(
		"OpenIsles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_SHOWN);
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

	IMG_Init(IMG_INIT_PNG);
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
			}
			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					quitGame = true;
					break;
				}
			}
		}

		// Frame zeichnen
		SDL_RenderClear(renderer);

		SDL_Rect rectDestination = { 0, 0, tileImgWidth, tileImgHeight };
		for (int y = 0; y < 768; y += tileImgHeight) {
			for (int x = 0; x < 1024; x += tileImgWidth) {
				rectDestination.x = x;
				rectDestination.y = y;
				SDL_RenderCopy(renderer, textureTestTile, NULL, &rectDestination);
			}
		}
		SDL_RenderPresent(renderer);

		Uint32 ticksAtLoopEnd = SDL_GetTicks();
		fps = 1000.0 / (ticksAtLoopEnd - ticksAtLoopStart);

		std::cout << "FPS = " << fps << std::endl;
	}

	SDL_DestroyTexture(textureTestTile);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
