#ifndef _AI_MGR_H
#define _AI_MGR_H

#include <lua.hpp>
#include "global.h"


/**
 * @brief Klasse, die die KI übernimmt
 */
class AiMgr : public ContextAware {

private:
    /**
     * @brief Lua-State
     */
    lua_State* lua;

public:
    AiMgr(const Context& context);
    ~AiMgr();

    /**
     * @brief Gibt der KI die Möglichkeit, sich zu initialisieren.
     * Die KI bekommt die initialen Bedingungen zum Spiel (Karte) mitgeteilt.
     */
    void init();

    /**
     * @brief Gibt der KI die Möglichkeit, Aktionen auszuführen
     */
    void update();

};

#endif
