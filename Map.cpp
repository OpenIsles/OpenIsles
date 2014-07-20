#include "GraphicsMgr.h"
#include "Map.h"

/**
 * Die Karte stellt sich wiefolgt dar:
 *
 *                 y=0
 *             --/  (0,0)   x=0
 *       y <--/      ---        \--
 *                --/   \--        \--> x
 *             --/         \--
 *          --/       x       \--(w-1,0)
 *    (0,h-1)  --\         /--
 *                --\   /--
 *                   ---
 *                (w-1,h-1)
 *
 *
 * Koordinatensysteme:
 *   Obenstehend sind die Kachel- oder Map-Koordinaten (mapCoords) dargestellt.
 *   Bildschirm-Koordinaten (screenCoords) sind durch ein Pixel-Koordinatensystem definiert. Hierbei wird die
 *   gesamte Karte um die Bildschirm-Koordinaten (0, 0) zentriert. Im Bild oben durch den Punkt x dargestellt.
 *   Die Bildschirm-Koordinate einer Kachel bestimmt die Position des Pixels in der Mitte der Kachel.
 */

// Aus main.cpp importiert
extern GraphicsMgr* graphicsMgr;


void Map::renderMap(SDL_Renderer* renderer) {
	// TODO muss sich die Anwendung irgendwie merken vbzw. aus dem Fenster holen (SDL_GetWindowSize(SDL_Window*,...))
	const int windowWidth = 1024;
	const int windowHeight = 768;

	// Kacheln rendern
	SDL_Rect rectDestination = { 0, 0, GraphicsMgr::TILE_WIDTH, GraphicsMgr::TILE_HEIGHT };
	for (unsigned int mapY = 0; mapY < height; mapY++) {
		for (unsigned int mapX = 0; mapX < width; mapX++) {
			int screenX, screenY;

			// Koordinatensystem wechseln
			screenX = mapX * (GraphicsMgr::TILE_WIDTH / 2) - mapY * (GraphicsMgr::TILE_WIDTH / 2);
			screenY = mapX * (GraphicsMgr::TILE_HEIGHT / 2) + mapY * (GraphicsMgr::TILE_HEIGHT / 2);

			// Kachelmittelpunkt als Referenzpunkt nehmen
			screenX -= (GraphicsMgr::TILE_WIDTH / 2);
			screenY -= (GraphicsMgr::TILE_HEIGHT / 2);

			// Im Fenster zentrieren
			screenX += (windowWidth / 2);
			screenY += (windowHeight / 2);

			// Scrolling-Offset anwenden
			screenX -= screenOffsetX;
			screenY -= screenOffsetY;

			rectDestination.x = screenX;
			rectDestination.y = screenY;
			SDL_RenderCopy(renderer, graphicsMgr->getTileTexture(0), NULL, &rectDestination);
		}
	}

	// Bildfläche anzeigen
	SDL_RenderPresent(renderer);

}

void Map::scroll(int screenOffsetX, int screenOffsetY) {
	this->screenOffsetX += screenOffsetX;
	this->screenOffsetY += screenOffsetY;
}
