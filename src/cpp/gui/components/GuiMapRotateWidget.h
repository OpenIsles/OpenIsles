#ifndef _GUI_MAP_ROTATE_WIDGET_H
#define _GUI_MAP_ROTATE_WIDGET_H

#include "Context.h"
#include "gui/components/GuiButton.h"


/**
 * @brief GUI-Element, was die Drehung der Karte anzeigt bzw. ändert
 */
class GuiMapRotateWidget : public GuiBase {

private:
    GuiButton buttonLeft;   ///< linker Button
    GuiButton buttonRight;  ///< rechter Button

public:
    /**
     * @brief Konstruktor.
     * @param context Dependency
     */
    GuiMapRotateWidget(const Context* const context);

    virtual ~GuiMapRotateWidget();

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) {
        // Nix zu tun, machen die Kinder alles
    }

    /**
     * @brief Callback, wenn die Karte außerhalb dieses Widgets gedreht wurde.
     */
    void onMapRotated();

private:
    /**
     * @brief Aktualisiert die Buttongrafiken mit der aktuellen Kartenansicht
     */
    void updateButtonGraphics();

};

#endif
