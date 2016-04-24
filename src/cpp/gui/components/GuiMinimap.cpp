#include <SDL.h>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "gui/components/GuiMinimap.h"
#include "map/coords/MapCoords.h"
#include "utils/Color.h"


// TODO nicht-quadratische Karten erlauben (Wir brauchen x/yScaleFactor).
// Info: Anno-Karten sind 500x350 Kacheln groß
// TODO Erlauben, guiElementBasedClipRect durch Methodenaufruf zu setzen (weil ein neues Spiel andere w/h-Map-Ratio hat)

GuiMinimap::GuiMinimap(const Context& context) :
    GuiStaticGraphicElement(context), guiElementBasedClipRect(40, 13, 166, 166) {

    setCoords(0, 0, 256, 200);
    setGraphic(context.graphicsMgr->getGraphicSet("minimap")->getStatic()->getGraphic());
}

GuiMinimap::~GuiMinimap() {
    // Minimap-Texture wegräumen
    if (minimapTexture != nullptr) {
        SDL_DestroyTexture(minimapTexture);
        minimapTexture = nullptr;
    }
}

void GuiMinimap::renderElement(IRenderer* renderer) {
    GuiStaticGraphicElement::renderElement(renderer);

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    Rect clipRect = {
        windowX + guiElementBasedClipRect.x, windowY + guiElementBasedClipRect.y,
        guiElementBasedClipRect.w, guiElementBasedClipRect.h
    };
    renderer->setClipRect(&clipRect);

    // Karte zeichnen
    SDL_Rect sdlMinimapClipRect = { clipRect.x, clipRect.y, clipRect.w, clipRect.h };
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_RenderCopy(sdlRealRenderer, minimapTexture, nullptr, &sdlMinimapClipRect);

    // Aktuellen Ausschnitt markieren
    renderer->setDrawColor(Color::minimapViewBorder);
    renderer->setDrawBlendMode(IRenderer::BLENDMODE_BLEND);
    SDL_RenderDrawLines(sdlRealRenderer, pointsCurrentClipping, 5);

    renderer->setDrawBlendMode(IRenderer::BLENDMODE_NONE);
    renderer->setClipRect(nullptr);
}

bool GuiMinimap::onEventElement(SDL_Event& event) {
    // Maustaste in der Minimap geklickt
    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT &&
        hitTest(event.button.x, event.button.y)) {

        onClickInMinimap(event.button.x, event.button.y);
        return false;
    }

    return true;
}

void GuiMinimap::onMapCoordsChanged() {
    Map* map = context.game->getMap();

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Aktuellen Ausschnitt bestimmen
    MapCoords mapCoordsTopLeft, mapCoordsTopRight, mapCoordsBottomLeft, mapCoordsBottomRight;
    MapCoordUtils::getMapCoordsInScreenEdges(
        *map, mapCoordsTopLeft, mapCoordsTopRight, mapCoordsBottomLeft, mapCoordsBottomRight);

    float scaleFactor = (float) map->getWidth() / (float) guiElementBasedClipRect.w;
    pointsCurrentClipping[0].x =
        windowX + guiElementBasedClipRect.x + (int) ((float) mapCoordsTopLeft.x() / scaleFactor);
    pointsCurrentClipping[0].y =
        windowY + guiElementBasedClipRect.y + (int) ((float) mapCoordsTopLeft.y() / scaleFactor);
    pointsCurrentClipping[1].x =
        windowX + guiElementBasedClipRect.x + (int) ((float) mapCoordsTopRight.x() / scaleFactor);
    pointsCurrentClipping[1].y =
        windowY + guiElementBasedClipRect.y + (int) ((float) mapCoordsTopRight.y() / scaleFactor);
    pointsCurrentClipping[2].x =
        windowX + guiElementBasedClipRect.x + (int) ((float) mapCoordsBottomRight.x() / scaleFactor);
    pointsCurrentClipping[2].y =
        windowY + guiElementBasedClipRect.y + (int) ((float) mapCoordsBottomRight.y() / scaleFactor);
    pointsCurrentClipping[3].x =
        windowX + guiElementBasedClipRect.x + (int) ((float) mapCoordsBottomLeft.x() / scaleFactor);
    pointsCurrentClipping[3].y =
        windowY + guiElementBasedClipRect.y + (int) ((float) mapCoordsBottomLeft.y() / scaleFactor);
    pointsCurrentClipping[4].x = pointsCurrentClipping[0].x;
    pointsCurrentClipping[4].y = pointsCurrentClipping[0].y;
}

void GuiMinimap::updateMinimapTexture() {
    Map* map = context.game->getMap();

    if (minimapTexture != nullptr) {
        SDL_DestroyTexture(minimapTexture);
    }

    float scaleFactor = (float) map->getWidth() / (float) guiElementBasedClipRect.w;

    // Karte zeichnen
    uint32_t* pixelData = new uint32_t[guiElementBasedClipRect.w * guiElementBasedClipRect.h];
    uint32_t* pixelPtr = pixelData;
    for (int yy = 0; yy < guiElementBasedClipRect.h; yy++) {
        for (int xx = 0; xx < guiElementBasedClipRect.w; xx++) {
            int mapX = (int) ((float) xx * scaleFactor);
            int mapY = (int) ((float) yy * scaleFactor);

            MapTile* mapTile = map->getMapTileAt(MapCoords(mapX, mapY));

            const MapTileConfig* mapTileConfig = mapTile->getMapTileConfig();
            Player* player = mapTile->player;

            *(pixelPtr++) = (mapTileConfig->isOceanOnMinimap()) ?
                            0x00000090 : (player != nullptr ?
                                        ((uint32_t) player->getColor()) : 0xff886040);
        }
    }

    // Daten zur Texture umwandeln
    IRenderer* const renderer = context.graphicsMgr->getRenderer();
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        pixelData, guiElementBasedClipRect.w, guiElementBasedClipRect.h, 32, guiElementBasedClipRect.w * 4,
        0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    minimapTexture = SDL_CreateTextureFromSurface(sdlRealRenderer, surface);
    SDL_FreeSurface(surface);
    delete[] pixelData;
}

void GuiMinimap::onClickInMinimap(int mouseX, int mouseY) {
    Map* map = context.game->getMap();

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    int xInClipRect = mouseX - (windowX + guiElementBasedClipRect.x);
    int yInClipRect = mouseY - (windowY + guiElementBasedClipRect.y);

    // Außerhalb des ClipRects nicht reagieren
    if (xInClipRect < 0 || yInClipRect < 0 ||
        xInClipRect >= guiElementBasedClipRect.w || yInClipRect >= guiElementBasedClipRect.h) {

        return;
    }

    float scaleFactor = (float) map->getWidth() / (float) guiElementBasedClipRect.w;

    int mapX = (int) ((float) xInClipRect  * scaleFactor);
    int mapY = (int) ((float) yInClipRect  * scaleFactor);

    map->setMapCoordsCentered(MapCoords(mapX, mapY));
    onMapCoordsChanged();
}