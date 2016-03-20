#include "config/BuildingCosts.h"
#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Player.h"


BuildingCosts::BuildingCosts(const Player* player, const Colony* colony, const ConfigMgr* configMgr) : BuildingCosts(
    player->coins,
    (int) colony->getGoods(configMgr->getGood("tools")).inventory,
    (int) colony->getGoods(configMgr->getGood("wood")).inventory,
    (int) colony->getGoods(configMgr->getGood("bricks")).inventory
) {}