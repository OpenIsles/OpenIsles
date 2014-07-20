#include <SDL.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		std::cerr << "SDL could not be initialized: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	atexit(SDL_Quit);
	std::cout << "SDL initialisert" << std::endl;

	SDL_Window *window = SDL_CreateWindow(
		"OpenIsles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "SDL could not create window: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cerr << "SDL could not create renderer: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		return EXIT_FAILURE;
	}

	// Mainloop
	bool quitGame = false;
	while (!quitGame) {
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
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
