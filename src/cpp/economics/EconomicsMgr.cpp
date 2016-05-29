#include <cassert>
#include "global.h"
#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Game.h"
#include "utils/DoubleToIntSequence.h"


EconomicsMgr::EconomicsMgr(const Context& context) : ContextAware(context) {
}

EconomicsMgr::~EconomicsMgr() {
}

void EconomicsMgr::updateProduction(Building* building) {
    const MapObjectType* mapObjectType = building->getMapObjectType();

    // Produziert eh nix bzw. Lager schon voll? Dann nix zu tun.
    if (!mapObjectType->buildingProduction.output.isUsed() ||
        building->productionSlots.output.isInventoryFull()) {

        return;
    }

    // Input-Güter reichen?
    auto areAllInputGoodsHere = [&]() {
        if (mapObjectType->buildingProduction.input.isUsed()) {
            if (building->productionSlots.input.inventory < mapObjectType->inputAmountForProduction) {
                return false;
            }

            if (mapObjectType->buildingProduction.input2.isUsed()) {
                if (building->productionSlots.input2.inventory < mapObjectType->input2AmountForProduction) {
                    return false;
                }
            }
        }
        return true;
    };

    if (!areAllInputGoodsHere()) {
        return;
    }

    // Waren die Waren grade frisch angeliefert?
    auto setNextGoodsProductionTicks = [&]() {
        building->hasRecievedAlInputGoodsSinceLastUpdate = true;
        building->nextGoodsProductionTicks = context.game->getTicks() +
            (unsigned long) (mapObjectType->secondsToProduce * TICKS_PER_SECOND);
    };

    if (!building->hasRecievedAlInputGoodsSinceLastUpdate) {
        // Ja. Dann beginnt ab jetzt die Produktionszeit.
        setNextGoodsProductionTicks();
        return;
    }

    // Prüfen, ob es Zeit ist, was zu produzieren
    if (context.game->getTicks() < building->nextGoodsProductionTicks) {
        return;
    }

    // Ja? Ok, dann produzieren
    if (mapObjectType->buildingProduction.input.isUsed()) {
        // TODO später für Goldschmiede explizit überprüfen. Das is das einzige Gebäude, was keine Ganzzahl hat
        // vermutlich muss man das eh mit DoubleToIntSequence lösen.
        assert(mapObjectType->inputAmountForProduction == int(mapObjectType->inputAmountForProduction));
        assert(mapObjectType->input2AmountForProduction == int(mapObjectType->input2AmountForProduction));

        building->productionSlots.input.decreaseInventory(mapObjectType->inputAmountForProduction);

        if (mapObjectType->buildingProduction.input2.isUsed()) {
            building->productionSlots.input2.decreaseInventory(mapObjectType->input2AmountForProduction);
        }
    }

    building->productionSlots.output.increaseInventory(1);

    // Sind noch genug Input-Güter da? Dann sofort das Helper-Flag setzen, damit sofort wieder die Uhr tickt
    if (areAllInputGoodsHere()) {
        setNextGoodsProductionTicks();
    } else {
        building->hasRecievedAlInputGoodsSinceLastUpdate = false;
    }
}

/**
 * @page finances Finanzen-Berechnung
 *
 * Nomenklatur
 * ===========
 *
 * - "Steuersatz" = Dezimalbruch aus Config-XML (bzw. Anno&nbsp;1602-COD-File)
 * - "Steuerprozent" = angezeigte Steuer-Prozentzahl im Spiel
 * - [\"Zyklus\"](@ref gameTicks) = 10 Sekunden Spielzeit. Das ist das Intervall, in welchem Finanzen
 *   aktualisiert werden.
 *
 * Überblick Formeln
 * =================
 *
 * Um die Berechnung der Finanzen möglichst genauso wie bei Anno&nbsp;1602 zu machen, wurden zwei Formeln
 * untersucht:
 * - a) Um mit dem Steuersatz aus dem COD-File hinzukommen, muss man die Basis auf "34% Steuer" (im Spiel angezeigt)
 *      setzen.
 * - b) Die Formel aus <a href="http://www.annozone.de/Charlie/Cod/numerik.html">AnnoZone</a>
 *
 * Beide Formeln können mittels einer Hilfstabelle (`doc/taxes-helper.ods`) verglichen werden.
 * Variante b) ist zwar genauer, aber komplizierter zu verstehen. Es wird deshalb Variante a) implementiert.
 *
 * Implementierung
 * ===============
 *
 * Die Formel ist einfach:
 * Der Steuersatz wird mit der Einwohnerzahl multipliziert und ergibt damit genau die Steuern für die Einstellung
 * "34% Steuern". Andere Einstellungen im Spiel werden darauf basierend gemacht. Mit 17% kommt genau die Hälfte raus,
 * bei 68% würde das doppelte rauskommen. Das Ergebnis wird ggf. abgerundet.
 *
 * Pro Bevölkerungsgruppe werden die Steuern wie obig berechnet. Am Ende werden alle (abgerundeten) Teilergebnisse
 * zusammengezählt.
 */

void EconomicsMgr::updatePlayerStatus() {
    const std::set<PopulationTier>& allPopulationTiers = context.configMgr->getAllPopulationTiers();
    const Game* game = context.game;

    unsigned long populationSumPerPlayer[4] = { 0, 0, 0, 0 };
    unsigned long taxesIncomeSumPerPlayer[4] = { 0, 0, 0, 0 };
    unsigned long operatingCostsSumPerPlayer[4] = { 0, 0, 0, 0 };

    // TODO überarbeiten: Wir brauchen später die Zahlen pro Kolonie zusätzlich zum Gesamt

    // Steuereinnahmen
    for (auto iter : game->getColonies()) {
        const Player* player = iter.first.first;
        const Colony* colony = iter.second;
        int playerIndex = player->getColorIndex();

        // Steuern: alle Bevölkerungsgruppen einzeln betrachten
        unsigned long taxesIncomeColony = 0;

        for (const PopulationTier& populationTier : allPopulationTiers) {
            const ColonyPopulationTier& colonyPopulationTier = colony->populationTiers.at(&populationTier);

            // double -> ulong-Cast zum Abschneiden der Dezimalstellen
            unsigned long taxesIncomeFromPopulationTier = (unsigned long) (
                (double) colonyPopulationTier.population *
                (double) populationTier.taxesPerInhabitant *
                    ((double) colonyPopulationTier.taxesPercent / (double) 34)
            );

            taxesIncomeColony += taxesIncomeFromPopulationTier;
        }

        taxesIncomeSumPerPlayer[playerIndex] += taxesIncomeColony;

        // Einwohner
        populationSumPerPlayer[playerIndex] += colony->population;
    }

    // Betriebskosten
    for (const MapObject* mapObject : game->getMap()->getMapObjects()) {
        const MapObjectType* mapObjectType = mapObject->getMapObjectType();

        unsigned char operatingCosts;
        operatingCosts = mapObjectType->operatingCosts.running;
        // TODO Stillgelegt-Status

        if (operatingCosts > 0) {
            int playerIndex = mapObject->getPlayer()->getColorIndex();
            operatingCostsSumPerPlayer[playerIndex] += operatingCosts;
        }
    }

    /// PlayerStatus aktualisieren
    for (int playerIndex = 0; playerIndex < 4; playerIndex++) {
        PlayerStatus& playerStatus = game->getPlayer(playerIndex)->playerStatus;
        playerStatus.population = populationSumPerPlayer[playerIndex];
        playerStatus.taxesIncome = taxesIncomeSumPerPlayer[playerIndex];
        playerStatus.operatingCosts = operatingCostsSumPerPlayer[playerIndex];
        playerStatus.balance = playerStatus.taxesIncome - playerStatus.operatingCosts;
    }
}

void EconomicsMgr::updateFinances() {
    for (int playerIndex = 0; playerIndex < 4; playerIndex++) {
        Player* player = context.game->getPlayer(playerIndex);

        // Steuereinnahmen
        unsigned long taxesIncome = (player->playerStatus.taxesIncome / 6); // abrunden
        player->coins += taxesIncome;

        // Betriebskosten
        unsigned long operatingCosts = (player->playerStatus.operatingCosts / 6); // abrunden
        player->coins -= operatingCosts;
    }
}

void EconomicsMgr::doGoodsConsumptionAndUpdatePopulationSatisfaction() {
    const ConfigMgr* configMgr = context.configMgr;

    // Nahrungsverbrauch
    const NeededGood& foodNeededGood = configMgr->getFoodGood();
    
    for (auto iter : context.game->getColonies()) {
        Colony* colony = iter.second;
        
        unsigned int sumHousesPopulation = 0;
        for (const PopulationTier& populationTier : configMgr->getAllPopulationTiers()) {
            sumHousesPopulation += colony->populationTiers.at(&populationTier).population;
        }

        // Verbrauch ermitteln (double in diskreten Integer ausgehend von der Spielzeit umrechnen)
        double foodConsumption = foodNeededGood.consumePerCycle * (double) sumHousesPopulation / (double) 100;
        std::array<int, 6> intSequence = DoubleToIntSequence::toIntSequence(foodConsumption);
        int cycleNr = (context.game->getTicks() / TICKS_PER_CYCLE) % 6;
        int foodConsumptionThisCycle = intSequence[cycleNr];

        // Verbrauchen
        GoodsSlot& foodGoodsSlot = colony->getGoods(foodNeededGood.good);

        assert(foodConsumptionThisCycle >= 0);
        int theoreticalInventoryAfter = foodGoodsSlot.inventory - foodConsumptionThisCycle;
        foodGoodsSlot.decreaseInventory((unsigned int) foodConsumptionThisCycle);

        if (theoreticalInventoryAfter >= 0) {
            for (const PopulationTier& populationTier : configMgr->getAllPopulationTiers()) {
                colony->populationTiers.at(&populationTier).populationSatisfaction = PopulationSatisfaction::GOOD;
            }
        }
        else {
            // Nahrung hat nicht gereicht!
            assert(foodGoodsSlot.inventory == 0);

            for (const PopulationTier& populationTier : configMgr->getAllPopulationTiers()) {
                colony->populationTiers.at(&populationTier).populationSatisfaction = PopulationSatisfaction::BAD;
            }

            // TODO Log-Meldung entfernen, wenn wir alles implementiert haben.
            Log::debug("Houses got %ut to less food. TODO starving", -theoreticalInventoryAfter);

            // TODO der Balken in der GUI muss nun abnehmen, Bevölkerungsrückgang bzw. Verhungern einsetzen
        }
    }

    // TODO Verbrauchsgüter
    // TODO öffentliche Gebäude
    // TODO vermutlich hier auch bevölkerungsverändernde Sachen machen (Zuzug, Auszug, Aufstieg, Abstieg, Verhungern)

    // Dem GuiMgr Bescheid geben, da sich ggf. was an der Zufriedenheit oder des Versorgungszustands geändert hat.
    // Dieser aktualisiert die Anzeige, wenn grade sichtbar.
#ifndef IN_TESTS
    context.guiMgr->onHouseInfoChanged();
#endif
}