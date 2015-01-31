#include <SDL.h>
#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiMinimap.h"
#include "map/coords/MapCoords.h"
#include "map/coords/ScreenCoords.h"
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
    // TODO Duplicate-Code refactorn, Rect (top, left, right, bottom) von Punkten einführen

    MapCoords mapCoordsTopLeft = MapCoordUtils::screenToMapCoords(ScreenCoords(
        screenOffsetX, screenOffsetY));
    MapCoords mapCoordsTopRight = MapCoordUtils::screenToMapCoords(ScreenCoords(
        (Consts::mapClipRect.w * screenZoom) + screenOffsetX, screenOffsetY));
    MapCoords mapCoordsBottomLeft = MapCoordUtils::screenToMapCoords(ScreenCoords(
        screenOffsetX, (Consts::mapClipRect.h * screenZoom) + screenOffsetY));
    MapCoords mapCoordsBottomRight = MapCoordUtils::screenToMapCoords(ScreenCoords(
        (Consts::mapClipRect.w * screenZoom) + screenOffsetX, (Consts::mapClipRect.h * screenZoom) + screenOffsetY));

    float scaleFactor = (float) map->getWidth() / (float) width;
    SDL_Point points[5] = {
        { windowX + (int) ((float) mapCoordsTopLeft.x() / scaleFactor),
            windowY + (int) ((float) mapCoordsTopLeft.y() / scaleFactor) },
        { windowX + (int) ((float) mapCoordsTopRight.x() / scaleFactor),
            windowY + (int) ((float) mapCoordsTopRight.y() / scaleFactor) },
        { windowX + (int) ((float) mapCoordsBottomRight.x() / scaleFactor),
            windowY + (int) ((float) mapCoordsBottomRight.y() / scaleFactor) },
        { windowX + (int) ((float) mapCoordsBottomLeft.x() / scaleFactor),
            windowY + (int) ((float) mapCoordsBottomLeft.y() / scaleFactor) }
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

    ScreenCoords screenCoords = MapCoordUtils::mapToScreenCoords(MapCoords(mapX, mapY));

    // zentrieren
    int screenZoom = map->getScreenZoom();
    screenCoords.subX((Consts::mapClipRect.w * screenZoom) / 2);
    screenCoords.subY((Consts::mapClipRect.h * screenZoom) / 2);

    map->setScreenOffset(screenCoords);
}