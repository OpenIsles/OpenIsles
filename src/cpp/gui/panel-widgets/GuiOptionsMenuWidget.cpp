#include <string>
#include "global.h"
#include "map/Structure.h"
#include "gui/components/GuiPushButton.h"
#include "gui/components/GuiPanelHeader.h"
#include "gui/panel-widgets/GuiOptionsMenuWidget.h"
#include "sound/sdl/SDLSoundMgr.h"


GuiOptionsMenuWidget::GuiOptionsMenuWidget(const Context& context) : 
    GuiPanelWidget(context), panelHeader(context), musicPushButton(context) {

    const std::string statusBarTextTurnMusicOn = _("Turn music on");
    const std::string statusBarTextTurnMusicOff = _("Turn music off");

    panelHeader.setPosition(0, 0);
    panelHeader.setText(_("Options"));
    panelHeader.setIconGraphic(nullptr);
    addChildElement(&panelHeader);

    musicPushButton.setGraphic(context.graphicsMgr->getGraphicSet("button-music")->getStatic()->getGraphic());
    musicPushButton.setGraphicPressed(context.graphicsMgr->getGraphicSet("button-music-pressed")->getStatic()->getGraphic());
    musicPushButton.setCoords(7, 345, 64, 64);
    musicPushButton.setStatusBarText(statusBarTextTurnMusicOn);
    musicPushButton.setOnClickFunction([this, &context, statusBarTextTurnMusicOff, statusBarTextTurnMusicOn]() {
        bool musicEnabled = musicPushButton.isActive();

        if (musicEnabled) {
            context.soundMgr->enableMusic();
            musicPushButton.setStatusBarText(statusBarTextTurnMusicOff);
        } else {
            context.soundMgr->disableMusic();
            musicPushButton.setStatusBarText(statusBarTextTurnMusicOn);
        }
    });
    addChildElement(&musicPushButton);
}
