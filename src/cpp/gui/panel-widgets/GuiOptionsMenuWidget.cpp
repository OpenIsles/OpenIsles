#include "Context.h"
#include "map/Structure.h"
#include "gui/Identifiers.h"
#include "gui/components/GuiPushButton.h"
#include "gui/panel-widgets/GuiOptionsMenuWidget.h"
#include "sound/sdl/SDLSoundMgr.h"


static const char* statusBarTextTurnMusicOn = "Musik einschalten";
static const char* statusBarTextTurnMusicOff = "Musik ausschalten";


GuiOptionsMenuWidget::GuiOptionsMenuWidget(const Context* const context) : GuiPanelWidget(context) {
    GuiPushButton* musicPushButton = new GuiPushButton(context);
    musicPushButton->setGraphic(context->graphicsMgr->getGraphicSet("button-music")->getStatic()->getGraphic());
    musicPushButton->setGraphicPressed(context->graphicsMgr->getGraphicSet("button-music-pressed")->getStatic()->getGraphic());
    musicPushButton->setCoords(7, 378, 64, 64);
    musicPushButton->setStatusBarText(statusBarTextTurnMusicOn);
    musicPushButton->setOnClickFunction([musicPushButton, context]() {
        bool musicEnabled = musicPushButton->isActive();

        if (musicEnabled) {
            context->soundMgr->enableMusic();
            musicPushButton->setStatusBarText(statusBarTextTurnMusicOff);
        } else {
            context->soundMgr->disableMusic();
            musicPushButton->setStatusBarText(statusBarTextTurnMusicOn);
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
