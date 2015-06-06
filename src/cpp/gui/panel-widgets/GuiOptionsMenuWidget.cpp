#include "Context.h"
#include "map/Structure.h"
#include "gui/Identifiers.h"
#include "gui/components/GuiPushButton.h"
#include "gui/panel-widgets/GuiOptionsMenuWidget.h"
#include "sound/sdl/SDLSoundMgr.h"


GuiOptionsMenuWidget::GuiOptionsMenuWidget(const Context* const context) : GuiPanelWidget(context) {
    GuiPushButton* musicPushButton = new GuiPushButton(context);
    musicPushButton->setGraphic(context->graphicsMgr->getGraphicSet("button-music")->getStatic()->getGraphic());
    musicPushButton->setGraphicPressed(context->graphicsMgr->getGraphicSet("button-music-pressed")->getStatic()->getGraphic());
    musicPushButton->setCoords(7, 378, 64, 64);
    musicPushButton->setOnClickFunction([this, context]() {
        bool musicEnabled = ((GuiPushButton*) context->guiMgr->findElement(GUI_ID_MUSIC_PUSH_BUTTON))->isActive();

        if (musicEnabled) {
            context->soundMgr->enableMusic();
        } else {
            context->soundMgr->disableMusic();
        }
    });
    context->guiMgr->registerElement(GUI_ID_MUSIC_PUSH_BUTTON, musicPushButton);
    addChildElement(musicPushButton);
}

void GuiOptionsMenuWidget::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    context->guiMgr->drawPanelHeader(windowX, windowY, "Einstellungen", nullptr);
}
