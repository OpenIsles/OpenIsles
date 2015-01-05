#include <SDL.h>
#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiMinimap.h"
#include "map/Map.h"
#include "utils/Consts.h"


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
    Map* map = context->game->getMap();
    int screenOffsetX = map->getScreenOffsetX();
    int screenOffsetY = map->getScreenOffsetY();
    int screenZoom = map->getScreenZoom();

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

    // Aktuellen Ausschnitt bestimmen
    // TODO Duplicate-Code refactorn, (x/y)-Tuple für MapCoords einführen, Rect (top, left, right, bottom) von Punkten einführen
    int mapXTopLeft, mapYTopLeft, mapXTopRight, mapYTopRight;
    int mapXBottomLeft, mapYBottomLeft, mapXBottomRight, mapYBottomRight;

    MapCoordUtils::screenToMapCoords(
        screenOffsetX, screenOffsetY,
        mapXTopLeft, mapYTopLeft);
    MapCoordUtils::screenToMapCoords(
        (Consts::mapClipRect.w * screenZoom) + screenOffsetX, screenOffsetY,
        mapXTopRight, mapYTopRight);
    MapCoordUtils::screenToMapCoords(
        screenOffsetX, (Consts::mapClipRect.h * screenZoom) + screenOffsetY,
        mapXBottomLeft, mapYBottomLeft);
    MapCoordUtils::screenToMapCoords(
        (Consts::mapClipRect.w * screenZoom) + screenOffsetX, (Consts::mapClipRect.h * screenZoom) + screenOffsetY,
        mapXBottomRight, mapYBottomRight);

    float scaleFactor = (float) map->getWidth() / (float) width;
    SDL_Point points[5] = {
        { windowX + (int) ((float) mapXTopLeft / scaleFactor),
            windowY + (int) ((float) mapYTopLeft / scaleFactor) },
        { windowX + (int) ((float) mapXTopRight / scaleFactor),
            windowY + (int) ((float) mapYTopRight / scaleFactor) },
        { windowX + (int) ((float) mapXBottomRight / scaleFactor),
            windowY + (int) ((float) mapYBottomRight / scaleFactor) },
        { windowX + (int) ((float) mapXBottomLeft / scaleFactor),
            windowY + (int) ((float) mapYBottomLeft / scaleFactor) }
    };
    points[4] = points[0];
    renderer->setDrawColor(Color(192, 128, 0, 255));
    renderer->setDrawBlendMode(IRenderer::BLENDMODE_BLEND);
    SDL_RenderDrawLines(sdlRealRenderer, points, 5);

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

            MapTile* mapTile = map->getMapTileAt(mapX, mapY);

            const MapTileConfig* mapTileConfig = context->configMgr->getMapTileConfig(mapTile->getTileIndex());
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

    int screenX, screenY;
    MapCoordUtils::mapToScreenCoords(mapX, mapY, screenX, screenY);

    // zentrieren
    int screenZoom = map->getScreenZoom();
    screenX -= (Consts::mapClipRect.w * screenZoom) / 2;
    screenY -= (Consts::mapClipRect.h * screenZoom) / 2;

    map->setScreenOffset(screenX, screenY);
}