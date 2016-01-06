#ifndef _GUI_COLONY_GOODS_WIDGET_H
#define _GUI_COLONY_GOODS_WIDGET_H

#include <SDL.h>
#include <unordered_map>
#include "global.h"
#include "gui/components/GuiGoodsSlotElement.h"
#include "gui/panel-widgets/GuiSelectedBuildingWidget.h"


/**
 * @brief GUI-Panel-Widget, dass den aktuellen Lagerbestand der Siedlung anzeigt
 */
class GuiColonyGoodsWidget : public GuiSelectedBuildingWidget {

private:
    /**
     * @brief GUI-Elemente für die Güter als Map.
     */
    std::unordered_map<const Good*, GuiGoodsSlotElement*> guiGoodsSlotElements;

    /**
     * @brief Siedlung, deren Waren angezeigt werden
     */
    const Colony* colony;

public:
    GuiColonyGoodsWidget(const Context& context);
    virtual ~GuiColonyGoodsWidget() override;

    /**
     * @brief Wird aufgerufen, wenn sich das markierte Map-Objekt ändert.
     * Das Widget korrigiert daraufhin den Zeiger zur Siedlung.
     *
     * @param newSelectedBuilding neues Gebäude, was nun selektiert ist
     */
    virtual void onSelectedMapBuildingChanged(const Building* newSelectedBuilding);

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);
};

#endif
