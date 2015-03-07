#include <SDL.h>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiMinimap.h"
#include "map/coords/MapCoords.h"


GuiMinimap::GuiMinimap(const Context* const context) : GuiBase(context) {
    setCoords(28, 28, 200, 200);
}

GuiMinimap::~GuiMinimap() {
    // Minimap-Texture wegräumen
    if (minimapTexture != nullptr) {
        SDL_DestroyTexture(minimapTexture);
        minimapTexture = nullptr;
    }
}

void GuiMinimap::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Nur die Kartenfläche vollmalen
    Rect fixedMinimapClipRect = Rect(windowX, windowY, width, height);
    fixedMinimapClipRect.y = renderer->getWindowHeight() - (windowY + height); // SDL misst das Rechteck von UNTEN, kp, warum und ob das ein Bug is
    renderer->setClipRect(&fixedMinimapClipRect);

    // Karte zeichnen
    SDL_Rect sdlMinimapClipRect = { windowX, windowY, width, height };
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_RenderCopy(sdlRealRenderer, minimapTexture, nullptr, &sdlMinimapClipRect);

    // Aktuellen Ausschnitt markieren
    renderer->setDrawColor(Color(192, 128, 0, 255));
    renderer->setDrawBlendMode(IRenderer::BLENDMODE_BLEND);
    SDL_RenderDrawLines(sdlRealRenderer, pointsCurrentClipping, 5);

    renderer->setDrawBlendMode(IRenderer::BLENDMODE_NONE);
    renderer->setClipRect(nullptr);
}

void GuiMinimap::onEventElement(SDL_Event& event) {
    // Maustaste in der Minimap geklickt
    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT &&
        hitTest(event.button.x, event.button.y)) {

        onClickInMinimap(event.button.x, event.button.y);
    }
}

void GuiMinimap::onMapCoordsChanged() {
    Map* map = context->game->getMap();

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Aktuellen Ausschnitt bestimmen
    MapCoords mapCoordsTopLeft, mapCoordsTopRight, mapCoordsBottomLeft, mapCoordsBottomRight;
    MapCoordUtils::getMapCoordsInScreenEdges(
        *map, mapCoordsTopLeft, mapCoordsTopRight, mapCoordsBottomLeft, mapCoordsBottomRight);

    float scaleFactor = (float) map->getWidth() / (float) width;
    pointsCurrentClipping[0].x = windowX + (int) ((float) mapCoordsTopLeft.x() / scaleFactor);
    pointsCurrentClipping[0].y = windowY + (int) ((float) mapCoordsTopLeft.y() / scaleFactor);
    pointsCurrentClipping[1].x = windowX + (int) ((float) mapCoordsTopRight.x() / scaleFactor);
    pointsCurrentClipping[1].y = windowY + (int) ((float) mapCoordsTopRight.y() / scaleFactor);
    pointsCurrentClipping[2].x = windowX + (int) ((float) mapCoordsBottomRight.x() / scaleFactor);
    pointsCurrentClipping[2].y = windowY + (int) ((float) mapCoordsBottomRight.y() / scaleFactor);
    pointsCurrentClipping[3].x = windowX + (int) ((float) mapCoordsBottomLeft.x() / scaleFactor);
    pointsCurrentClipping[3].y = windowY + (int) ((float) mapCoordsBottomLeft.y() / scaleFactor);
    pointsCurrentClipping[4].x = pointsCurrentClipping[0].x;
    pointsCurrentClipping[4].y = pointsCurrentClipping[0].y;
}

void GuiMinimap::updateMinimapTexture() {
    Map* map = context->game->getMap();

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    if (minimapTexture != nullptr) {
        SDL_DestroyTexture(minimapTexture);
    }

    float scaleFactor = (float) map->getWidth() / (float) width;

    // Karte zeichnen
    uint32_t* pixelData = new uint32_t[width * height];
    uint32_t* pixelPtr = pixelData;
    for (int yy = 0, screenY = windowY + y; yy < height; yy++, screenY++) {
        for (int xx = 0, screenX = windowX + x; xx < width; xx++, screenX++) {
            int mapX = (int) ((float) xx * scaleFactor);
            int mapY = (int) ((float) yy * scaleFactor);

            MapTile* mapTile = map->getMapTileAt(MapCoords(mapX, mapY));

            const MapTileConfig* mapTileConfig = mapTile->getMapTileConfig();
            Player* player = mapTile->player;

            *(pixelPtr++) =
                (mapTileConfig->isOcean) ? 0x000090 : (player != nullptr ? ((uint32_t) player->getColor()) : 0x008000);
        }
    }

    // Daten zur Texture umwandeln
    IRenderer* const renderer = context->graphicsMgr->getRenderer();
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixelData, width, height, 32, width * 4, 0, 0, 0, 0);
    minimapTexture = SDL_CreateTextureFromSurface(sdlRealRenderer, surface);
    SDL_FreeSurface(surface);
    delete[] pixelData;
}

void GuiMinimap::onClickInMinimap(int mouseX, int mouseY) {
    Map* map = context->game->getMap();

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    int xInMinimap = mouseX - windowX;
    int yInMinimap = mouseY - windowY;

    float scaleFactor = (float) map->getWidth() / (float) width;

    int mapX = (int) ((float) xInMinimap  * scaleFactor);
    int mapY = (int) ((float) yInMinimap  * scaleFactor);

    map->setMapCoordsCentered(MapCoords(mapX, mapY));
    onMapCoordsChanged();
}