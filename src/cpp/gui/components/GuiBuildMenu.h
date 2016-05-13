#ifndef _GUI_BUILD_MENU_H
#define _GUI_BUILD_MENU_H

#include <SDL.h>
#include <list>
#include "global.h"
#include "graphics/graphic/IGraphic.h"
#include "gui/components/GuiBase.h"
#include "gui/components/GuiButton.h"


/**
 * Baumenü, was sich dynamisch in der Höhe vergrößern kann
 */
class GuiBuildMenu : public GuiBase {

private:
    const IGraphic* graphicPanelTop;    ///< Referenz auf die Panel-Grafik (oberes Stück)
    const IGraphic* graphicPanel;       ///< Referenz auf die Panel-Grafik (wiederholendes Mittelstück)
    const IGraphic* graphicPanelBottom; ///< Referenz auf die Panel-Grafik (unteres Stück)

    std::list<GuiButton*> buttons;      ///< Buttons

    /**
     * @brief Anzahl Zeilen, die das Baumenü hat. Das gibt die Größe vor
     */
    int rows = 1;

public:
    GuiBuildMenu(const Context& context);
    virtual ~GuiBuildMenu() override;

    /**
     * @brief Ändert die Buttons, die im Menü angezeigt werden
     * @param newButtons Liste mit den Buttons (von oben links nach unten rechts).
     *                   Nicht freigeschaltete Buttons sind in der Liste als `nullptr` enthalten.
     */
    void setButtons(std::list<const MapObjectType*>& newButtons);

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) override;
};

#endif
