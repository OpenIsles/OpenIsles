#ifndef _GUI_PLAYER_STATUS_WIDGET_H
#define _GUI_PLAYER_STATUS_WIDGET_H

#include "Context.h"
#include "game/Player.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
 * @brief GUI-Panel-Widget, dass Spielerstatus anzeigt
 */
class GuiPlayerStatusWidget : public GuiPanelWidget {

private:
    /**
     * @brief Spieler, dessen Status angezeigt wird
     */
    const Player* player;

public:
    GuiPlayerStatusWidget(const Context* const context);
    virtual ~GuiPlayerStatusWidget() override;

    void setPlayer(const Player* player) {
        this->player = player;
    }

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);
};

#endif
