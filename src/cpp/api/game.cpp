#include "api/api-global.h"
#include "game/Game.h"
#include "game/Player.h"
#include "map/buildqueue/BuildOperation.h"


DEFINE_LUA_FUNCTION(getGameTicks) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    lua_pushnumber(lua, context->game->getTicks());
    return 1;
}

DEFINE_LUA_FUNCTION(build) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    // Parameter prüfen

    int playerIndex = luaL_checkinteger(lua, 1);
    Player* player = context->game->getPlayer(playerIndex - 1);
    if (player == nullptr) {
        return luaL_error(lua, _("Invalid playerIndex").c_str());
    }

    const char* mapObjectTypeName = luaL_checkstring(lua, 2);
    const MapObjectType* mapObjectType = context->configMgr->getMapObjectType(mapObjectTypeName);
    if (mapObjectType == nullptr) {
        return luaL_error(lua, _("Invalid mapObjectType").c_str());
    }

    int mapX = luaL_checkinteger(lua, 3);
    int mapY = luaL_checkinteger(lua, 4);
    MapCoords mapCoords(mapX, mapY);

    MapTile* mapTile = context->game->getMap()->getMapTileAt(mapCoords);
    if (mapTile == nullptr) {
        return luaL_error(lua, _("Invalid mapCoords").c_str());
    }

    const char* viewName = luaL_checkstring(lua, 5);
    FourthDirection view = Direction::fromString(viewName);
    if (view != Direction::SOUTH &&
        view != Direction::EAST &&
        view != Direction::NORTH &&
        view != Direction::WEST) {

        return luaL_error(lua, _("Invalid viewName").c_str());
    }

    // Passt soweit. Nun gucken, ob wir bauen können
    BuildOperation buildOperation(*context, *player); // TODO nur der aktuelle Spieler darf bauen (BuildOperation kann das aber auch noch nicht)
    buildOperation.requestBuild(mapCoords, mapObjectType, view);

    if (buildOperation.getResult().result != BuildOperationResult::OK) {
        return luaL_error(lua, _("Cannot build there").c_str());
    }

    buildOperation.doBuild();
    return 0;
}