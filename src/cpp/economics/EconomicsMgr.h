#ifndef _ECONOMICS_MGR_H
#define _ECONOMICS_MGR_H

#include "Context.h"
#include "map/Building.h"


/**
 * @brief Manager, der die Wirtschaft übernimmt
 */
class EconomicsMgr : public ContextAware {

public:
    EconomicsMgr(const Context* const context);
    ~EconomicsMgr();

    /**
     * @brief Führt die Produktion für ein Gebäude durch. Es wird nur für jene vergangene Ticks produziert, für die
     * alle Eingabewaren da waren und auch nur solange das Ausgabewarenlager nicht voll ist.
     *
     * Algorithmus:
     * Es wird ermittelt, wie viel verbraucht würde, wenn in allen vergangenen Ticks die Voraussetzungen erfüllt sind.
     * Dann wird ermittelt, wie viele Ticks lang pro Slot die Voraussetzungen wirklich erfüllt sind. Mit dem Minimum
     * dieser Tick-Werte wird dann die Produktion durchgeführt.
     *
     * @param building Gebäude, für welches die Produktion durchgeführt werden soll
     */
    void updateProduction(Building* building);
};

#endif
