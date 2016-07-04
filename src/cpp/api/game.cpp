#include <cassert>
#include "api/api-global.h"
#include "game/Game.h"
#include "game/Player.h"
#include "map/buildqueue/BuildOperation.h"
#include "map/Isle.h"
#include "map/Map.h"
#include "utils/StringFormat.h"


/**
 * Baut einen Lua-Table für ein Map-Objekt auf.
 * Nach dem Aufruf befindet sich ein Wert (der Table) zusätzlich auf dem Lua-Stack.
 *
 * @param lua Lua-State
 * @param mapObjectFixed Map-Objekt, das wir nach Lua reichen wollen
 */
static void pushMapObjectOnLuaStack(lua_State* lua, const MapObjectFixed* mapObjectFixed) {
    const MapCoords& mapCoords = mapObjectFixed->getMapCoords();

    int player = (mapObjectFixed->getPlayer() != nullptr) ? (mapObjectFixed->getPlayer()->getPlayerIndex() + 1) : 0;
    const char* view;

    if (mapObjectFixed->getView() == Direction::SOUTH) {
        view = "south";
    } else if (mapObjectFixed->getView() == Direction::EAST) {
        view = "east";
    } else if (mapObjectFixed->getView() == Direction::NORTH) {
        view = "north";
    } else if (mapObjectFixed->getView() == Direction::WEST) {
        view = "west";
    } else {
        assert (false);
        view = nullptr;
    }

    lua_createtable(lua, 0, 5);
    lua_pushinteger(lua, (lua_Integer) mapCoords.x());
    lua_setfield(lua, -2, "x");
    lua_pushinteger(lua, (lua_Integer) mapCoords.y());
    lua_setfield(lua, -2, "y");
    lua_pushinteger(lua, (lua_Integer) player);
    lua_setfield(lua, -2, "player");
    lua_pushstring(lua, mapObjectFixed->getMapObjectType()->name.c_str());
    lua_setfield(lua, -2, "type");
    lua_pushstring(lua, view);
    lua_setfield(lua, -2, "view");
}

/**
 * Liest eine Map-Koordinate vom Lua-Stack. Erwartet einen Table mit Keys x und y.
 * Der Stack wird nicht verändert.
 *
 * @param lua Lua-State
 * @param stackPosOfArgument Position auf dem Stack
 * @return Map-Koordinate
 */
static MapCoords readMapCoordsFromStack(lua_State* lua, int stackPosOfArgument) {
    luaL_checktype(lua, stackPosOfArgument, LUA_TTABLE);

    lua_getfield(lua, stackPosOfArgument, "y");
    lua_getfield(lua, stackPosOfArgument, "x");

    lua_Integer x = luaL_checkinteger(lua, -1);
    lua_Integer y = luaL_checkinteger(lua, -2);
    lua_pop(lua, 2);

    return MapCoords(x, y);
}


DEFINE_LUA_FUNCTION(getGameTicks) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    lua_pushnumber(lua, context->game->getTicks());
    return 1;
}

DEFINE_LUA_FUNCTION(getMap) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    const Map* map = context->game->getMap();
    lua_createtable(lua, 0, map->getWidth() * map->getHeight() + 2);

    lua_pushinteger(lua, (lua_Integer) map->getWidth());
    lua_setfield(lua, -2, "width");

    lua_pushinteger(lua, (lua_Integer) map->getHeight());
    lua_setfield(lua, -2, "height");

    for (int y = 0; y < map->getHeight(); y++) {
        for (int x = 0; x < map->getWidth(); x++) {
            const MapTile* mapTile = map->getMapTileAt({x, y});

            std::string key = toString(x) + "," + toString(y);

            lua_pushinteger(lua, (lua_Integer) mapTile->getMapTileConfig()->mapTileType);
            lua_setfield(lua, -2, key.c_str());
        }
    }
    return 1;
}

DEFINE_LUA_FUNCTION(getIsles) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    const Map* map = context->game->getMap();
    const std::list<Isle*>& isles = map->getIsles();

    lua_createtable(lua, int(isles.size()), 0);

    int i = 1;
    for (const Isle* isle : isles) {
        const MapCoords& mapCoords = isle->getMapCoords();

        lua_createtable(lua, 0, 4);
        lua_pushinteger(lua, (lua_Integer) isle->getWidth());
        lua_setfield(lua, -2, "width");
        lua_pushinteger(lua, (lua_Integer) isle->getHeight());
        lua_setfield(lua, -2, "height");
        lua_pushinteger(lua, (lua_Integer) mapCoords.x());
        lua_setfield(lua, -2, "x");
        lua_pushinteger(lua, (lua_Integer) mapCoords.y());
        lua_setfield(lua, -2, "y");

        lua_rawseti(lua, -2, (lua_Integer) i++);
    }
    return 1;
}

DEFINE_LUA_FUNCTION(getShips) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    const Map* map = context->game->getMap();
    const std::list<MapObject*>& allMapObjects = map->getMapObjects();

    lua_newtable(lua);

    int i = 1;
    for (const MapObject* mapObject : allMapObjects) {
        const Ship* ship = dynamic_cast<const Ship*>(mapObject);
        if (ship == nullptr) {
            continue;
        }

        const DoubleMapCoords& mapCoords = ship->getMapCoords();

        lua_createtable(lua, 0, 4);
        lua_pushnumber(lua, mapCoords.x());
        lua_setfield(lua, -2, "x");
        lua_pushnumber(lua, mapCoords.y());
        lua_setfield(lua, -2, "y");
        lua_pushinteger(lua, (lua_Integer) (ship->getPlayer()->getPlayerIndex() + 1));
        lua_setfield(lua, -2, "player");
        lua_pushstring(lua, ship->getName().c_str());
        lua_setfield(lua, -2, "name");

        lua_rawseti(lua, -2, (lua_Integer) i++);
    }
    return 1;
}

DEFINE_LUA_FUNCTION(getMapObjectsFixed) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    const Map* map = context->game->getMap();
    const std::list<MapObject*>& allMapObjects = map->getMapObjects();

    lua_newtable(lua);

    int i = 1;
    for (const MapObject* mapObject : allMapObjects) {
        const MapObjectFixed* mapObjectFixed = dynamic_cast<const MapObjectFixed*>(mapObject);
        if (mapObjectFixed == nullptr) {
            continue;
        }

        pushMapObjectOnLuaStack(lua, mapObjectFixed);
        lua_rawseti(lua, -2, (lua_Integer) i++);
    }
    return 1;
}

DEFINE_LUA_FUNCTION(getMapObjectFixedAt) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    MapCoords mapCoords = readMapCoordsFromStack(lua, 1);

    const Map* map = context->game->getMap();
    MapObjectFixed* mapObjectFixed = map->getMapObjectFixedAt(mapCoords);

    if (mapObjectFixed != nullptr) {
        pushMapObjectOnLuaStack(lua, mapObjectFixed);
    } else {
        lua_pushnil(lua);
    }

    return 1;
}

DEFINE_LUA_FUNCTION(getColonyAt) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    MapCoords mapCoords = readMapCoordsFromStack(lua, 1);
    const Colony* colony = context->game->getColony(mapCoords);

    if (colony == nullptr) {
        lua_pushnil(lua);
        return 1;
    }

    lua_createtable(lua, 0, 5);
    lua_pushstring(lua, colony->name.c_str());
    lua_setfield(lua, -2, "name");
    lua_pushinteger(lua, colony->population);
    lua_setfield(lua, -2, "population");
    
    unsigned int capacity = 0;
    
    lua_createtable(lua, 0, 1);
    const std::list<const Good*>& allGoods = context->configMgr->getAllGoodsOrdered();
    for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++) {
        const Good* good = *iter;
        const GoodsSlot& goodsSlot = colony->getGoods(good);
        
        // Kapazität ist für alle Slots gleich; einmalig merken
        if (capacity == 0) {
            capacity = goodsSlot.capacity;
        }
        
        lua_pushinteger(lua, goodsSlot.inventory);
        lua_setfield(lua, -2, good->name.c_str());
    }
    lua_setfield(lua, -2, "goods");

    lua_pushinteger(lua, capacity);
    lua_setfield(lua, -2, "goodsCapacity");

    MapTile* mapTile = context->game->getMap()->getMapTileAt(mapCoords);
    assert (mapTile->player != nullptr);
    lua_pushinteger(lua, (lua_Integer) (mapTile->player->getPlayerIndex() + 1));
    lua_setfield(lua, -2, "player");

    return 1;
}

DEFINE_LUA_FUNCTION(build) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    // Parameter prüfen

    int playerIndex = luaL_checkinteger(lua, 1);
    Player* player = context->game->getPlayer(playerIndex - 1);
    if (player == nullptr) {
        return luaL_error(lua, _("Invalid playerIndex.").c_str());
    }
    if (!player->isAI()) {
        return luaL_error(lua, _("AI may only play for AI players.").c_str());
    }

    const char* mapObjectTypeName = luaL_checkstring(lua, 2);
    const MapObjectType* mapObjectType = context->configMgr->getMapObjectType(mapObjectTypeName);
    if (mapObjectType == nullptr) {
        return luaL_error(lua, _("Invalid mapObjectType.").c_str());
    }

    int mapX = luaL_checkinteger(lua, 3);
    int mapY = luaL_checkinteger(lua, 4);
    MapCoords mapCoords(mapX, mapY);

    MapTile* mapTile = context->game->getMap()->getMapTileAt(mapCoords);
    if (mapTile == nullptr) {
        return luaL_error(lua, _("Invalid mapCoords.").c_str());
    }

    const char* viewName = luaL_checkstring(lua, 5);
    FourthDirection view = Direction::fromString(viewName);
    if (view != Direction::SOUTH &&
        view != Direction::EAST &&
        view != Direction::NORTH &&
        view != Direction::WEST) {

        return luaL_error(lua, _("Invalid viewName.").c_str());
    }

    // Passt soweit. Nun gucken, ob wir bauen können
    BuildOperation buildOperation(*context, *player); // TODO nur der aktuelle Spieler darf bauen (BuildOperation kann das aber auch noch nicht)
    buildOperation.requestBuild(mapCoords, mapObjectType, view);

    if (buildOperation.getResult().result != BuildOperationResult::OK) {
        return luaL_error(lua, _("Cannot build there.").c_str());
    }

    buildOperation.doBuild();
    return 0;
}

DEFINE_LUA_FUNCTION(demolish) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    // Parameter prüfen

    int playerIndex = luaL_checkinteger(lua, 1);
    Player* player = context->game->getPlayer(playerIndex - 1);
    if (player == nullptr) {
        return luaL_error(lua, _("Invalid playerIndex.").c_str());
    }
    if (!player->isAI()) {
        return luaL_error(lua, _("AI may only play for AI players.").c_str());
    }

    int mapX = luaL_checkinteger(lua, 2);
    int mapY = luaL_checkinteger(lua, 3);
    MapCoords mapCoords(mapX, mapY);

    MapObjectFixed* mapObjectFixed = context->game->getMap()->getMapObjectFixedAt(mapCoords);
    if (mapObjectFixed == nullptr) {
        return luaL_error(lua, _("Nothing to demolish at these mapCoords.").c_str());
    }

    // Passt soweit. Nun gucken, ob wir abreißen können
    BuildOperation buildOperation(*context, *player); // TODO nur der aktuelle Spieler darf bauen (BuildOperation kann das aber auch noch nicht)
    buildOperation.requestDemolish(*mapObjectFixed);

    if (buildOperation.getResult().result != BuildOperationResult::OK) {
        return luaL_error(lua, _("Cannot demolish there.").c_str());
    }

    buildOperation.doBuild();
    return 0;
}