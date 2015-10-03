#ifndef _GUI_PRODUCTION_SLOTS_ELEMENT_H
#define _GUI_PRODUCTION_SLOTS_ELEMENT_H

#include "game/ProductionSlot.h"
#include "graphics/renderer/IRenderer.h"
#include "gui/components/GuiGoodsSlotElement.h"
#include "gui/components/GuiStaticElement.h"
#include "Context.h"

/**
 * @brief Statisches Element, was die Produktion eines Gebäude anzeigt.
 *
 * Das Element kann in einen von 4 Modi gesetzt werden
 * - nur Produktion
 * - ein Input-Gut wird verarbeitet (Input + Pfeil + Output)
 * - zwei Input-Güter werden verarbeitet (Input + Plus + Input2 + Pfeil + Output)
 * - keine Produktion (gar nix anzeigen)
 *
 * Auf die bis zu drei Güter-Elemente kann per getGoodsSlotElement() zugegriffen werden.
 */
class GuiProductionSlotsElement : public GuiStaticElement {

private:
    // childElements
    GuiGoodsSlotElement* goodsSlotInput;    ///< Gütersymbol: verbraute Güter
    GuiGoodsSlotElement* goodsSlotInput2;   ///< Gütersymbol: weitere verbraute Güter
    GuiGoodsSlotElement* goodsSlotOutput;   ///< Gütersymbol: produzierte Güter
    GuiStaticElement* productionPlus;       ///< Symbol Pluszeichen
    GuiStaticElement* productionArrow;      ///< Symbol Pfeil

public:
    GuiProductionSlotsElement(const Context* const context);
    virtual ~GuiProductionSlotsElement() override;

    /**
     * @brief Ändert das Layout der Komponente, indem Güter-Elemente hinzugefügt/entfernt werden.
     *
     * @param productionsSlotsToShow Elemente, die angezeigt werden sollen. Bitmaske aus ProductionSlot-Enums.
     */
    void setProductionSlots(unsigned char productionsSlotsToShow);

    /**
     * @brief Liefert das GuiGoodsSlotElement für einen Slot zurück.
     *
     * @param productionSlot welcher ProductionSlot soll zurückgeliefert werden
     * @return GUI-Element des ProductionSlots
     */
    GuiGoodsSlotElement* getGoodsSlotElement(const ProductionSlot& productionSlot);

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) override {
        // nix zu tun; machen alles die Kinder
    }

};

#endif
