#ifndef _LUA_H
#define _LUA_H

#include <lua.hpp>
#include "global.h"


extern const luaL_Reg apiMethods[];

/**
 * @param context Context, den mit dem Lua-State assoziiert wird, damit Lua auf unsere Inhalte zugreifen kann
 * @return Erzeugt einen neuen Lua-Context
 */
lua_State* createNewLuaState(const Context& context);

#endif