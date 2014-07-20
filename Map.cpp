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
// TODO später in einen Graphics-Manager stecken
extern int tileImgWidth;
extern int tileImgHeight;
extern SDL_Texture* textureTestTile;

void Map::renderMap(SDL_Renderer* renderer) {
	// TODO muss sich die Anwendung irgendwie merken vbzw. aus dem Fenster holen (SDL_GetWindowSize(SDL_Window*,...))
	const int windowWidth = 1024;
	const int windowHeight = 768;

	// Kacheln rendern
	SDL_Rect rectDestination = { 0, 0, tileImgWidth, tileImgHeight };
	for (unsigned int mapY = 0; mapY < height; mapY++) {
		for (unsigned int mapX = 0; mapX < width; mapX++) {
			int screenX, screenY;

			// Koordinatensystem wechseln
			screenX = mapX * (tileImgWidth / 2) - mapY * (tileImgWidth / 2);
			screenY = mapX * (tileImgHeight / 2) + mapY * (tileImgHeight / 2);

			// Kachelmittelpunkt als Referenzpunkt nehmen
			screenX -= (tileImgWidth / 2);
			screenY -= (tileImgHeight / 2);

			// Im Fenster zentrieren
			screenX += (windowWidth / 2);
			screenY += (windowHeight / 2);

			// Scrolling-Offset anwenden
			screenX -= screenOffsetX;
			screenY -= screenOffsetY;

			rectDestination.x = screenX;
			rectDestination.y = screenY;
			SDL_RenderCopy(renderer, textureTestTile, NULL, &rectDestination);
		}
	}

	// Bildfläche anzeigen
	SDL_RenderPresent(renderer);

}

void Map::scroll(int screenOffsetX, int screenOffsetY) {
	this->screenOffsetX += screenOffsetX;
	this->screenOffsetY += screenOffsetY;
}
