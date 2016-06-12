#ifndef _AI_MGR_H
#define _AI_MGR_H

#include <lua.h>
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
     * @brief Gibt der KI die Möglichkeit, Aktionen auszuführen
     */
    void update();

};

#endif
