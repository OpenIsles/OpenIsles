#ifndef _GUI_DUMMY_WIDGET_H
#define _GUI_DUMMY_WIDGET_H

#include <SDL_render.h>
#include "global.h"
#include "gui/components/GuiPanelHeader.h"
#include "gui/panel-widgets/GuiPanelWidget.h"


/**
 * @brief GUI-Panel-Widget, was nur einen Dummy-Text anzeigt. Es wird am Ende entfernt, wenn alles fertig is.
 * TODO entfern mich, wenn nicht mehr gebraucht
 */
class GuiDummyWidget : public GuiPanelWidget {

private:
    GuiPanelHeader* panelHeader;

public:
    GuiDummyWidget(const Context& context);
    virtual ~GuiDummyWidget();

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);

};

#endif