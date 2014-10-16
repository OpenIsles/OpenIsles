#ifndef _ECONOMICS_MGR_H
#define _ECONOMICS_MGR_H

#include <map>

struct Structure;

/**
 * @brief Manager, der die Wirtschaft übernimmt
 */
class EconomicsMgr {

public:
    EconomicsMgr();
    ~EconomicsMgr();

    /**
     * @brief Aktualisiert eine Struktur. D.h. es werden Waren produziert, die zuständigen Männchen und Tierchen
     * bewegt und was sonst noch alles zum "Leben" der Struktur dazugehört.
     * @param structure Struktur, die aktualisiert werden soll
     */
    void update(Structure* structure);

};

#endif
