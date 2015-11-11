#ifndef _GUI_SELECTED_BUILDING_WIDGET_H
#define _GUI_SELECTED_BUILDING_WIDGET_H

#include <SDL.h>
#include <string>
#include "Context.h"
#include "gui/components/GuiProductionSlotsElement.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
 * @brief GUI-Widget mit den Informationen des aktuell ausgewählten Gebäudes.
 */
class GuiSelectedBuildingWidget : public GuiPanelWidget {

private:
    /**
     * @brief GUI-Element für produzierte Güter
     */
    GuiProductionSlotsElement guiProductionSlotsElement;

    // childElements der guiProductionSlotsElement
    GuiGoodsSlotElement* goodsSlotInput;    ///< Gütersymbol: verbraute Güter
    GuiGoodsSlotElement* goodsSlotInput2;   ///< Gütersymbol: weitere verbraute Güter
    GuiGoodsSlotElement* goodsSlotOutput;   ///< Gütersymbol: produzierte Güter

public:
    GuiSelectedBuildingWidget(const Context* const context);
    virtual ~GuiSelectedBuildingWidget() override {}

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);

    /**
     * @brief Wird aufgerufen, wenn sich das markierte Map-Objekt ändert.
     * Das Widget passt darauf hin ggf. die Komponenten drauf an.
     *
     * @param newSelectedBuilding neues Gebäude, was nun selektiert ist
     */
    void onSelectedMapBuildingChanged(const Building* newSelectedBuilding);

private:
    /**
     * @brief Aktualisiert den Statusleisten-Text für ein Input-GuiGoodsSlotElement.
     * Es wird der Text "...vorrat (reicht für weitere ...t ...)" gesetzt.
     *
     * @param goodsSlotElement GUI-Element, dessen Statusbar-Text geändert wird
     * @param inputAmountForProduction Info aus der Gebäude-Konfiguration,
     *                                 wie viele Güter pro 1t Output verbraucht werden
     * @param secondsToProduce Info aus der Gebäude-Konfiguration, wie viele Sekunden es dauert,
     *                         um 1t Output herzustellen
     * @param inputInventory Info aus dem konkreten Gebäude, wie viele Güter zur Verfügung stehen
     * @param inputLabel Info aus der Gebäude-Konfiguration, wie das Input-Gut heißt
     * @param outputLabel Info aus der Gebäude-Konfiguration, wie das Output-Gut heißt
     */
    void updateInputSlotStatusBarText(
        GuiGoodsSlotElement* goodsSlotElement,
        double inputAmountForProduction, double secondsToProduce, double inputInventory,
        const std::string& inputLabel, const std::string& outputLabel);
};

#endif
